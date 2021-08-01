#include "Renderer/RayTracingRenderer.h"

void RayTracingRenderer::Init()
{

}

void RayTracingRenderer::Destroy()
{
    m_Scene = nullptr;
}

void RayTracingRenderer::Update()
{

}

void RayTracingRenderer::Render()
{

}

void RayTracingRenderer::SetScene(GLScenePtr scene)
{
    m_Scene = scene;
}
