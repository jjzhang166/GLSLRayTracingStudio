#pragma once

#include "Base/Renderer.h"

#include "Core/Shader.h"
#include "Core/Program.h"

#include <vector>

class PBRRenderer : public Renderer
{
public:

    PBRRenderer()
        : m_Scene(nullptr)
        , m_PBRShader(nullptr)
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

    GLScenePtr      m_Scene;
    GLProgramPtr    m_PBRShader;
};
