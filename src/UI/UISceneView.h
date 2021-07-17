#pragma once

#include "Base/SceneView.h"

#include "Math/Vector2.h"

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

    void DrawMenuBar();
    void DrawAboutUI();
    void HandleMoving();

private:

    ImGuiIO*    m_ImGuiIO;

    Vector2     m_MenuBarMousePos;
    bool        m_MenuBarDragging;

    bool        m_ShowingAbout;
};
