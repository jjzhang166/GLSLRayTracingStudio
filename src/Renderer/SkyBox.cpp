#include "Renderer/SkyBox.h"
#include "Renderer/IBLSampler.h"

#include "Misc/FileMisc.h"

SkyBox::SkyBox()
    : m_Program(nullptr)
    , m_VertexBuffer(nullptr)
    , m_IndexBuffer(nullptr)
    , m_VAO(0)
{

}

SkyBox::~SkyBox()
{

}

void SkyBox::Init()
{
    // program
    {
        std::shared_ptr<GLShader> vertShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/skybox.vert", GL_VERTEX_SHADER);
        std::shared_ptr<GLShader> fragShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/skybox.frag", GL_FRAGMENT_SHADER);
        std::vector<std::shared_ptr<GLShader>> shaders;
        shaders.push_back(vertShader);
        shaders.push_back(fragShader);
        m_Program = new GLProgram(shaders);
    }

    // geometry
    {
        float vertices[] =
        {
            // position        // nomrla         // uv
            // Front face
            -1.0, -1.0,  1.0,  0.0, 0.0, -1.0,  0.0, 0.0,
             1.0, -1.0,  1.0,  0.0, 0.0, -1.0,  1.0, 0.0,
             1.0,  1.0,  1.0,  0.0, 0.0, -1.0,  1.0, 1.0,
            -1.0,  1.0,  1.0,  0.0, 0.0, -1.0,  0.0, 1.0,

            // Back face
            -1.0, -1.0, -1.0,  0.0, 0.0,  1.0,  0.0, 0.0,
            -1.0,  1.0, -1.0,  0.0, 0.0,  1.0,  1.0, 0.0,
             1.0,  1.0, -1.0,  0.0, 0.0,  1.0,  1.0, 1.0,
             1.0, -1.0, -1.0,  0.0, 0.0,  1.0,  0.0, 1.0,

            // Top face
            -1.0,  1.0, -1.0,  0.0, -1.0, 0.0,  0.0, 0.0,
            -1.0,  1.0,  1.0,  0.0, -1.0, 0.0,  1.0, 0.0,
             1.0,  1.0,  1.0,  0.0, -1.0, 0.0,  1.0, 1.0,
             1.0,  1.0, -1.0,  0.0, -1.0, 0.0,  0.0, 1.0,

            // Bottom face
            -1.0, -1.0, -1.0,  0.0,  1.0, 0.0,  0.0, 0.0,
             1.0, -1.0, -1.0,  0.0,  1.0, 0.0,  1.0, 0.0,
             1.0, -1.0,  1.0,  0.0,  1.0, 0.0,  1.0, 1.0,
            -1.0, -1.0,  1.0,  0.0,  1.0, 0.0,  0.0, 1.0,

            // Right face
            1.0, -1.0, -1.0,  -1.0, 0.0, 0.0,   0.0, 0.0,
            1.0,  1.0, -1.0,  -1.0, 0.0, 0.0,   1.0, 0.0,
            1.0,  1.0,  1.0,  -1.0, 0.0, 0.0,   1.0, 1.0,
            1.0, -1.0,  1.0,  -1.0, 0.0, 0.0,   0.0, 1.0,

            // Left face
            -1.0, -1.0, -1.0,  1.0, 0.0, 0.0,   0.0, 0.0,
            -1.0, -1.0,  1.0,  1.0, 0.0, 0.0,   1.0, 0.0,
            -1.0,  1.0,  1.0,  1.0, 0.0, 0.0,   1.0, 1.0,
            -1.0,  1.0, -1.0,  1.0, 0.0, 0.0,   0.0, 1.0
        };
        m_VertexBuffer = new VertexBuffer();
        m_VertexBuffer->Upload((uint8*)(&vertices[0]), sizeof(vertices));
        
        uint32 indices[] =
        {
            0,  2,  1,      0,  3,  2,    // front
            4,  6,  5,      4,  7,  6,    // back
            8,  10, 9,      8,  11, 10,   // top
            12, 14, 13,     12, 15, 14,   // bottom
            16, 18, 17,     16, 19, 18,   // right
            20, 22, 21,     20, 23, 22,   // left
        };
        m_IndexBuffer = new IndexBuffer();
        m_IndexBuffer->Upload((uint8*)(&indices[0]), sizeof(indices));

        glGenVertexArrays(1, &m_VAO);
        glBindVertexArray(m_VAO);
        glBindBuffer(m_VertexBuffer->Target(), m_VertexBuffer->Object());

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
        glEnableVertexAttribArray(0);
        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)12);
        glEnableVertexAttribArray(1);
        // uv
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)24);
        glEnableVertexAttribArray(2);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void SkyBox::Destroy()
{
    if (m_VAO != 0)
    {
        glDeleteVertexArrays(1, &m_VAO);
        m_VAO = 0;
    }

    if (m_VertexBuffer)
    {
        delete m_VertexBuffer;
        m_VertexBuffer = nullptr;
    }

    if (m_IndexBuffer)
    {
        delete m_IndexBuffer;
        m_IndexBuffer = nullptr;
    }

    if (m_Program)
    {
        delete m_Program;
        m_Program = nullptr;
    }
}

void SkyBox::Draw(CameraPtr camera, IBLSampler* ibl)
{
    Matrix4x4 model;
    model.SetIdentity();
    Matrix4x4 view = camera->GetView();
    view.SetOrigin(Vector3(0.0f, 0.0f, 0.0f));
    Matrix4x4 mvp = model * view * camera->GetProjection();

    m_Program->Active();
    m_Program->SetUniform4x4f("_MVP", mvp);
    m_Program->SetUniform1f("_Exposure", ibl->exposure);
    m_Program->SetUniform1f("_GammaValue", ibl->gammaValue);
    m_Program->SetUniform1f("_EnvironmentLod", ibl->environmentLod);
    m_Program->SetUniform1f("_EnvironmentIntensity", ibl->environmentIntensity);
    m_Program->SetTexture("_GGXEnvSampler", GL_TEXTURE_CUBE_MAP, ibl->GGXTexture(), 0);

    glCullFace(GL_FRONT);
    glBindVertexArray(m_VAO);
    glBindBuffer(m_IndexBuffer->Target(), m_IndexBuffer->Object());
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glBindBuffer(m_IndexBuffer->Target(), 0);
    glCullFace(GL_BACK);

    m_Program->Deactive();
}
