#pragma once

#include "Base/Base.h"
#include "Base/Renderer.h"

#include <vector>

class RayTracingRenderer : public Renderer
{
public:

    RayTracingRenderer()
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

    void AddScene(std::shared_ptr<Scene3D> scene) override;

private:

    std::vector<Scene3DPtr>     m_Scenes;
};
