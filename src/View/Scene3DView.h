#pragma once

#include "Base/SceneView.h"
#include "Base/Renderer.h"
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

    void AddRenderer(RendererPtr renderer);

private:

    RendererArray   m_Renderers;

};
