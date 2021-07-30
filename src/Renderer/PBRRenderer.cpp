#include "Renderer/PBRRenderer.h"

void PBRRenderer::Init()
{

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

}

void PBRRenderer::SetScene(GLScenePtr scene)
{
    m_Scene = scene;
}
