#include "Renderer/IBLSampler.h"
#include "Misc/FileMisc.h"
#include "Math/Math.h"

IBLSampler::IBLSampler()
    : m_InputTexture(0)
    , m_CubeTexture(0)

    , m_LambertianTexture(0)
    , m_GGXTexture(0)
    , m_SheenTexture(0)

    , m_GGXLutTexture(0)
    , m_CharlieLutTexture(0)

    , m_FrameBuffer(0)

    , m_ProgramIBL(nullptr)
    , m_ProgramCube(nullptr)
    , m_Quad(nullptr)

    , m_TextureSize(256)
    , m_GGXSampleCount(1024)
    , m_LambertianSampleCount(2048)
    , m_SheenSamplCount(64)

    , m_LodBias(0.0f)
    , m_LutResolution(1024)
    , m_LowestMipLevel(4)
    , m_MipMapCount(0)

{

}

IBLSampler::~IBLSampler()
{
    
}

GLuint IBLSampler::CreateCubemapTexture(bool withMipmaps)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    for (int32 i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA32F, m_TextureSize, m_TextureSize, 0, GL_RGBA, GL_FLOAT, nullptr);
    }

    if (withMipmaps)
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    }
    else
    {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return texture;
}

void IBLSampler::Destroy()
{
    m_HDRImage = nullptr;

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

    if (m_LambertianTexture != 0)
    {
        glDeleteTextures(1, &m_LambertianTexture);
        m_LambertianTexture = 0;
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

    if (m_GGXLutTexture != 0)
    {
        glDeleteTextures(1, &m_GGXLutTexture);
        m_GGXLutTexture = 0;
    }

    if (m_CharlieLutTexture != 0)
    {
        glDeleteTextures(1, &m_CharlieLutTexture);
        m_CharlieLutTexture = 0;
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

    if (m_Quad != nullptr)
    {
        delete m_Quad;
        m_Quad = nullptr;
    }
}

void IBLSampler::Init(HDRImagePtr hdrImage)
{
    m_HDRImage = hdrImage;

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

    // fullscreen
    {
        m_Quad = new Quad();
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
        m_CubeTexture       = CreateCubemapTexture(true);
        m_LambertianTexture = CreateCubemapTexture(false);
        m_GGXTexture        = CreateCubemapTexture(true);
        m_SheenTexture      = CreateCubemapTexture(true);

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_GGXTexture);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, m_SheenTexture);
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        m_MipMapCount = MMath::FloorToInt(MMath::Log2((float)m_TextureSize)) + 1 - m_LowestMipLevel;
    }

    // process
    PanoramaToCubeMap();
    CubeMapToLambertian();
    CubeMapToGGX();
    CubeMapToSheen();

    // reset frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void IBLSampler::PanoramaToCubeMap()
{
    for (int32 i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_CubeTexture, 0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeTexture);
        
        glViewport(0, 0, m_TextureSize, m_TextureSize);
        glScissor(0, 0, m_TextureSize, m_TextureSize);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_ProgramCube->Active();
        m_ProgramCube->SetUniform1i("_CurrentFace", i);
        m_ProgramCube->SetTexture("_InputTexture", GL_TEXTURE_2D, m_InputTexture, 0);

        m_Quad->Draw();

        m_ProgramCube->Deactive();
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeTexture);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

void IBLSampler::CubeMapToLambertian()
{
    ApplyFilter(0, 0.0f, 0, m_LambertianTexture, m_LambertianSampleCount);
}

void IBLSampler::CubeMapToGGX()
{
    for (int32 currentMipLevel = 0; currentMipLevel < m_MipMapCount; ++currentMipLevel)
    {
        float roughness = currentMipLevel * 1.0f / (m_MipMapCount - 1);
        ApplyFilter(1, roughness, currentMipLevel, m_GGXTexture, m_GGXSampleCount);
    }
}

void IBLSampler::CubeMapToSheen()
{
    for (int32 currentMipLevel = 0; currentMipLevel < m_MipMapCount; ++currentMipLevel)
    {
        float roughness = currentMipLevel * 1.0f / (m_MipMapCount - 1);
        ApplyFilter(2, roughness, currentMipLevel, m_SheenTexture, m_SheenSamplCount);
    }
}

void IBLSampler::ApplyFilter(int32 distribution, float roughness, int32 targetMipLevel, GLuint targetTexture, int32 sampleCount)
{
    int32 currentTextureSize = m_TextureSize >> targetMipLevel;

    for (int32 i = 0; i < 6; ++i)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_FrameBuffer);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, targetTexture, targetMipLevel);
        glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture);

        glViewport(0, 0, currentTextureSize, currentTextureSize);
        glScissor(0, 0, currentTextureSize, currentTextureSize);
        glClearColor(0.0, 0.0, 0.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_ProgramIBL->Active();
        m_ProgramIBL->SetTexture("_InputCubeMap", GL_TEXTURE_CUBE_MAP, m_CubeTexture, 0);
        m_ProgramIBL->SetUniform1f("_Roughness", roughness);
        m_ProgramIBL->SetUniform1i("_SampleCount", sampleCount);
        m_ProgramIBL->SetUniform1i("_Width", m_TextureSize);
        m_ProgramIBL->SetUniform1f("_LodBias", m_LodBias);
        m_ProgramIBL->SetUniform1i("_Distribution", distribution);
        m_ProgramIBL->SetUniform1i("_CurrentFace", i);
        m_ProgramIBL->SetUniform1i("_GeneratingLUT", 0);

        m_Quad->Draw();

        m_ProgramIBL->Deactive();
    }
}
