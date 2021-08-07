#include "Renderer/IBLSampler.h"
#include "Misc/FileMisc.h"
#include "Math/Math.h"

IBLSampler::IBLSampler()
    : m_VertexBuffer(nullptr)
    , m_IndexBuffer(nullptr)

    , m_InputTexture(0)
    , m_CubeTexture(0)
    , m_LambertTexture(0)
    , m_GGXTexture(0)
    , m_SheenTexture(0)
    , m_FrameBuffer(0)

    , m_VAO(0)

    , m_ProgramIBL(nullptr)
    , m_ProgramCube(nullptr)

    , m_SampleSize(512)
    , m_SampleCount(128)
    , m_LodBias(0.0f)
    , m_MipMapCount(0)
{
    m_MipMapCount = MMath::FloorToInt(MMath::Log2((float)m_SampleSize)) + 1;
}

IBLSampler::~IBLSampler()
{
    
}

GLuint IBLSampler::CreateCubemapTexture(bool withMipmaps, int32 size)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (int32 i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB32F, size, size, 0, GL_RGB, GL_FLOAT, nullptr);
    }

    if (withMipmaps)
    {
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R,     GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

void IBLSampler::Destroy()
{
    if (m_InputTexture != 0)
    {
        glDeleteTextures(1, &m_InputTexture);
        m_InputTexture = 0;
    }

    if (m_CubeTexture != 0)
    {
        glDeleteTextures(1, &m_CubeTexture);
        m_CubeTexture = 0;
    }

    if (m_LambertTexture != 0)
    {
        glDeleteTextures(1, &m_LambertTexture);
        m_LambertTexture = 0;
    }

    if (m_GGXTexture != 0)
    {
        glDeleteTextures(1, &m_GGXTexture);
        m_GGXTexture = 0;
    }

    if (m_SheenTexture != 0)
    {
        glDeleteTextures(1, &m_SheenTexture);
        m_SheenTexture = 0;
    }

    if (m_FrameBuffer != 0)
    {
        glDeleteFramebuffers(1, &m_FrameBuffer);
        m_FrameBuffer = 0;
    }

    if (m_ProgramIBL != nullptr)
    {
        delete m_ProgramIBL;
        m_ProgramIBL = nullptr;
    }

    if (m_ProgramCube != nullptr)
    {
        delete m_ProgramCube;
        m_ProgramCube = nullptr;
    }

    if (m_VertexBuffer != nullptr)
    {
        delete m_VertexBuffer;
        m_VertexBuffer = nullptr;
    }

    if (m_IndexBuffer != nullptr)
    {
        delete m_IndexBuffer;
        m_IndexBuffer = nullptr;
    }
}

void IBLSampler::Init(HDRImagePtr hdrImage)
{
    // ibl shader
    {
        std::shared_ptr<GLShader> vertShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/ibl/Fullscreen.vert", GL_VERTEX_SHADER);
        std::shared_ptr<GLShader> fragShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/ibl/IBLFiltering.frag", GL_FRAGMENT_SHADER);
        std::vector<std::shared_ptr<GLShader>> shaders;
        shaders.push_back(vertShader);
        shaders.push_back(fragShader);
        m_ProgramIBL = new GLProgram(shaders);
    }

    // cubemap shader
    {
        std::shared_ptr<GLShader> vertShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/ibl/Fullscreen.vert", GL_VERTEX_SHADER);
        std::shared_ptr<GLShader> fragShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/ibl/PanoramaToCubemap.frag", GL_FRAGMENT_SHADER);
        std::vector<std::shared_ptr<GLShader>> shaders;
        shaders.push_back(vertShader);
        shaders.push_back(fragShader);
        m_ProgramCube = new GLProgram(shaders);
    }

    // hdr texture
    {
        glGenTextures(1, &m_InputTexture);
        glBindTexture(GL_TEXTURE_2D, m_InputTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, hdrImage->width, hdrImage->height, 0, GL_RGB, GL_FLOAT, hdrImage->hdrRGB.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // frame buffer
    {
        glGenFramebuffers(1, &m_FrameBuffer);
    }

    // ibl textures
    {
        m_CubeTexture    = CreateCubemapTexture(true, m_SampleSize);
        m_LambertTexture = CreateCubemapTexture(false, m_SampleSize);
        m_GGXTexture     = CreateCubemapTexture(true, m_SampleSize);
        m_SheenTexture   = CreateCubemapTexture(true, m_SampleSize);
    }

    // quad buffer
    {
        float vertices[] =
        {
            -1.0f,  1.0f,  0.0f,  0.0f,  1.0f,
             1.0f,  1.0f,  0.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -1.0f, -1.0f,  0.0f,  0.0f,  0.0f
        };
        m_VertexBuffer = new VertexBuffer();
        m_VertexBuffer->Upload((uint8*)(&vertices[0]), sizeof(vertices));

        uint32 indices[] =
        {
            0, 1, 2, 0, 2, 3
        };
        m_IndexBuffer = new IndexBuffer();
        m_IndexBuffer->Upload((uint8*)(&indices[0]), sizeof(indices));

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        glBindBuffer(m_VertexBuffer->Target(), m_VertexBuffer->Object());
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (GLvoid*)12);
        glEnableVertexAttribArray(1);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    // process
    PanoramaToCubeMap();
    CubeMapToLambertian();
    CubeMapToGGX();
    CubeMapToSheen();

    // reset frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLSampler::Draw()
{
    glBindVertexArray(m_VAO);
    glBindBuffer(m_IndexBuffer->Target(), m_IndexBuffer->Object());
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(m_IndexBuffer->Target(), 0);
}

void IBLSampler::PanoramaToCubeMap()
{
    m_ProgramCube->Active();

    for (int32 i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubeTexture, 0);
        
        glViewport(0, 0, m_SampleSize, m_SampleSize);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        m_ProgramCube->SetUniform1i("_CurrentFace", i);
        m_ProgramCube->SetTexture("_Panorama", GL_TEXTURE_2D, m_InputTexture, 0);

        Draw();
    }

    m_ProgramCube->Deactive();

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeTexture);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLSampler::CubeMapToLambertian()
{
    ApplyFilter(0, 0.0f, 0, m_LambertTexture);
}

void IBLSampler::CubeMapToGGX()
{
    for (int32 currentMipLevel = 0; currentMipLevel < m_MipMapCount; ++currentMipLevel)
    {
        float roughness = currentMipLevel * 1.0f / (m_MipMapCount - 1);
        ApplyFilter(1, roughness, currentMipLevel, m_GGXTexture);
    }
}

void IBLSampler::CubeMapToSheen()
{
    for (int32 currentMipLevel = 0; currentMipLevel < m_MipMapCount; ++currentMipLevel)
    {
        float roughness = currentMipLevel * 1.0f / (m_MipMapCount - 1);
        ApplyFilter(2, roughness, currentMipLevel, m_SheenTexture);
    }
}

void IBLSampler::ApplyFilter(int32 distribution, float roughness, int32 targetMipLevel, GLuint targetTexture)
{
    int32 currentTextureSize = m_SampleSize >> targetMipLevel;

    m_ProgramIBL->Active();

    for (int32 i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, targetTexture, targetMipLevel);
        glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture);
        glViewport(0, 0, currentTextureSize, currentTextureSize);
        glClearColor(1.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        m_ProgramIBL->SetTexture("_CubeMap", GL_TEXTURE_CUBE_MAP, m_CubeTexture, 0);
        m_ProgramIBL->SetUniform1f("_Roughness", roughness);
        m_ProgramIBL->SetUniform1i("_SampleCount", m_SampleCount);
        m_ProgramIBL->SetUniform1i("_Width", m_SampleSize);
        m_ProgramIBL->SetUniform1f("_LodBias", m_LodBias);
        m_ProgramIBL->SetUniform1i("_Distribution", distribution);
        m_ProgramIBL->SetUniform1i("_CurrentFace", i);

        Draw();
    }

    m_ProgramIBL->Deactive();
}
