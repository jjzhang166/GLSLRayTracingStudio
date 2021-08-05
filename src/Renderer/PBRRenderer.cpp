#include "Renderer/PBRRenderer.h"
#include "Misc/FileMisc.h"
#include "Core/Scene.h"

void PBRRenderer::Init()
{
    // shader
    {
        GLShaderPtr vertShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/pbr/vert.glsl", GL_VERTEX_SHADER);
        GLShaderPtr fragShader = std::make_shared<GLShader>(GetRootPath() + "assets/shaders/pbr/frag.glsl", GL_FRAGMENT_SHADER);
        std::vector<GLShaderPtr> shaders;
        shaders.push_back(vertShader);
        shaders.push_back(fragShader);
        m_PBRShader = std::make_shared<GLProgram>(shaders);
    }
}

void PBRRenderer::Destroy()
{
    m_Scene = nullptr;
}

void PBRRenderer::Update()
{

}

void PBRRenderer::Render()
{
    if (m_Scene->Renderers().size() == 0 || m_Scene->GetCamera() == nullptr)
    {
        return;
    }

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
        const auto& model       = nodes[renderNode.nodeID]->GlobalTransform();

        // mvp loc
        {
            auto mvp  = model * view * proj;
            GLint loc = glGetUniformLocation(m_PBRShader->Object(), "mvp");
            glUniformMatrix4fv(loc, 1, GL_FALSE, &(mvp.m[0][0]));
        }

        glBindVertexArray(vao);
        glBindBuffer(indexBuffer->Target(), indexBuffer->Object());
        glDrawElements(GL_TRIANGLES, (GLsizei)(mesh->indices.size()), GL_UNSIGNED_INT, (void*)(0));
        glBindVertexArray(0);
    }

    m_PBRShader->Deactive();
}

void PBRRenderer::SetScene(GLScenePtr scene)
{
    m_Scene = scene;
}
