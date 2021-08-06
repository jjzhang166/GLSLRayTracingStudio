#pragma once

#include "Base/Renderer.h"
#include "Renderer/SkyBox.h"

#include "Core/Shader.h"
#include "Core/Program.h"

#include <vector>

class PBRRenderer : public Renderer
{
public:

    PBRRenderer()
        : m_Scene(nullptr)
        , m_PBRShader(nullptr)
        , m_Skybox(nullptr)
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

    void RenderSkybox();

    void RenderOpaqueEntites();

    void RenderBlendEntites();

private:

    GLScenePtr      m_Scene;
    GLProgram*      m_PBRShader;
    SkyBox*         m_Skybox;
};
