#include "Base/GLWindow.h"
#include "View/Scene3DView.h"
#include "Renderer/PBRRenderer.h"
#include "Renderer/RayTracingRenderer.h"

#include <glad/glad.h>

Scene3DView::Scene3DView(std::shared_ptr<GLWindow> window)
    : SceneView(window)
{

}

Scene3DView::~Scene3DView()
{

}

bool Scene3DView::Init()
{
    AddRenderer(std::make_shared<PBRRenderer>());
    AddRenderer(std::make_shared<RayTracingRenderer>());
    return true;
}

void Scene3DView::Destroy()
{

}

void Scene3DView::OnUpdate()
{

}

void Scene3DView::OnRender()
{
    auto rect = Window()->GetUISceneView()->ViewPort3D();

    glViewport(rect.x, rect.y, rect.w, rect.h);
    glScissor(rect.x, rect.y, rect.w, rect.h);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Scene3DView::AddRenderer(RendererPtr renderer)
{
    auto it = std::find(m_Renderers.begin(), m_Renderers.end(), renderer);
    if (it == m_Renderers.end())
    {
        m_Renderers.push_back(renderer);
    }
}
