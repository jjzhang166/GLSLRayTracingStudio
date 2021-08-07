#pragma once

#include "Base/Renderer.h"

#include <vector>

class RayTracingRenderer : public Renderer
{
public:

    RayTracingRenderer()
        : m_Scene(nullptr)
    {

    }

    virtual ~RayTracingRenderer()
    {

    }

    RendererType Type() override
    {
        return RendererType::Raytracing;
    }

    void Init() override;

    void Destroy() override;

    void Update() override;

    void Render() override;

    void SetScene(GLScenePtr scene) override;

private:

    GLScenePtr  m_Scene;
};
