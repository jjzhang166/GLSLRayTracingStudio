#pragma once

#include "Base/SceneView.h"
#include "Math/Vector2.h"
#include "Math/Rectangle.h"

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

    void UpdatePanelRects();

    void DrawMenuBar();

    void DrawAboutUI();

    void HandleMoving();

    void DrawProjectPanel();

    void DrawPropertyPanel();

    void DrawAssetsPanel();

private:

    ImGuiIO*    m_ImGuiIO;

    Vector2     m_MenuBarMousePos;
    bool        m_MenuBarDragging;

    bool        m_ShowingAbout;

    float       m_PanelProjectWidth;
    float       m_PanelPropertyWidth;
    float       m_PanelAssetsWidth;

    Rectangle   m_PanelProjectRect;
    Rectangle   m_PanelPropertyRect;
    Rectangle   m_PanelAssetsRect;
};
