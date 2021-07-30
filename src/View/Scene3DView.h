#pragma once

#include "Base/SceneView.h"

#include "Renderer/PBRRenderer.h"
#include "Renderer/RayTracingRenderer.h"

#include "Math/Vector2.h"
#include "Math/Rectangle2D.h"

class Scene3DView : public SceneView
{
public:

    Scene3DView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene);

    virtual ~Scene3DView();

    bool Init() override;

    void Destroy() override;

    void OnUpdate() override;

    void OnRender() override;

private:

    std::shared_ptr<PBRRenderer>        m_PBRRenderer;
    std::shared_ptr<RayTracingRenderer> m_RayRenderer;
    
};
