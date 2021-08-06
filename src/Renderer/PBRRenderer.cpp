#include "Renderer/PBRRenderer.h"
#include "Misc/FileMisc.h"
#include "Core/Scene.h"

void PBRRenderer::Init()
{
    // shader
    {
        std::shared_ptr<GLShader> vertShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/pbr/vert.glsl", GL_VERTEX_SHADER);
        std::shared_ptr<GLShader> fragShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/pbr/frag.glsl", GL_FRAGMENT_SHADER);
        std::vector<std::shared_ptr<GLShader>> shaders;
        shaders.push_back(vertShader);
        shaders.push_back(fragShader);
        m_PBRShader = new GLProgram(shaders);
    }

    m_Skybox = new SkyBox();
    m_Skybox->Init();
}

void PBRRenderer::Destroy()
{
    m_Skybox->Destroy();
    delete m_Skybox;
    m_Skybox = nullptr;

    delete m_PBRShader;
    m_PBRShader = nullptr;
}

void PBRRenderer::Update()
{

}

void PBRRenderer::RenderSkybox()
{
    m_Skybox->Draw(m_Scene->GetCamera(), m_Scene->IBLs()[0]);
}

void PBRRenderer::RenderOpaqueEntites()
{
    const auto& nodes        = m_Scene->Nodes();
    const auto& meshes       = m_Scene->Meshes();
    const auto& renderers    = m_Scene->Renderers();
    const auto& vaos         = m_Scene->VAOs();
    const auto& indexBuffers = m_Scene->IndexBuffers();

    const Matrix4x4& view = m_Scene->GetCamera()->GetView();
    const Matrix4x4& proj = m_Scene->GetCamera()->GetProjection();

    m_PBRShader->Active();

    for (size_t i = 0; i < renderers.size(); ++i)
    {
        const auto& renderNode  = renderers[i];
        const auto& mesh        = meshes[renderNode.meshID];
        const auto& indexBuffer = indexBuffers[renderNode.meshID];
        const auto& vao         = vaos[renderNode.meshID];
        const auto& model       = nodes[renderNode.nodeID]->GetGlobalTransform();
        Matrix4x4 mvp           = model * view * proj;

        m_PBRShader->SetUniform4x4f("_MVP", mvp);

        glBindVertexArray(vao);
        glBindBuffer(indexBuffer->Target(), indexBuffer->Object());
        glDrawElements(GL_TRIANGLES, (GLsizei)(mesh->indices.size()), GL_UNSIGNED_INT, (void*)(0));
        glBindVertexArray(0);
    }

    m_PBRShader->Deactive();
}

void PBRRenderer::RenderBlendEntites()
{


}

void PBRRenderer::Render()
{
    RenderOpaqueEntites();
    RenderBlendEntites();
    RenderSkybox();
}

void PBRRenderer::SetScene(GLScenePtr scene)
{
    m_Scene = scene;
}
