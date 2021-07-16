#pragma once

#include "Base/SceneView.h"

#include <memory>

struct ImGuiIO;

class GLWindow;

class UISceneView : public SceneView
{
public:

    UISceneView(std::shared_ptr<GLWindow> window);

    ~UISceneView();

    bool Init() override;

    void Destroy() override;

    void OnUpdate() override;

    void OnRender() override;

    ImGuiIO& ImIO() const;

private:

    ImGuiIO* m_ImGuiIO;
};
