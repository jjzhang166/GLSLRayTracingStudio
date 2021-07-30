#pragma once

#include "Base/Renderer.h"

#include <vector>

class PBRRenderer : public Renderer
{
public:

    PBRRenderer()
    {

    }

    virtual ~PBRRenderer()
    {

    }

    RendererType Type() override
    {
        return RendererType::PBR;
    }

    void Init() override;

    void Destroy() override;

    void Update() override;

    void Render() override;

    void SetScene(GLScenePtr scene) override;

private:

    GLScenePtr  m_Scene;
};
