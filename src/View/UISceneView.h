#pragma once

#include "Base/SceneView.h"
#include "Math/Vector2.h"
#include "Math/Rectangle2D.h"

#include <memory>

struct ImGuiIO;
class GLWindow;

class UISceneView : public SceneView
{
public:

    UISceneView(std::shared_ptr<GLWindow> window);

    virtual ~UISceneView();

    bool Init() override;

    void Destroy() override;

    void OnUpdate() override;

    void OnRender() override;

    ImGuiIO& ImIO() const;

    FORCEINLINE Rectangle2D ViewPort3D() const
    {
        return m_PanelScene3DRect;
    }

private:

    void UpdatePanelRects();

    void DrawMenuBar();

    void DrawAboutUI();

    void HandleMoving();

    void DrawProjectPanel();

    void DrawPropertyPanel();

    void DrawAssetsPanel();

    void DrawMessageUI();

private:

    ImGuiIO*        m_ImGuiIO;

    Vector2         m_MenuBarMousePos;
    bool            m_MenuBarDragging;

    bool            m_ShowingAbout;

    float           m_PanelProjectWidth;
    float           m_PanelPropertyWidth;
    float           m_PanelAssetsWidth;

    Rectangle2D       m_PanelProjectRect;
    Rectangle2D       m_PanelPropertyRect;
    Rectangle2D       m_PanelAssetsRect;
    Rectangle2D       m_PanelScene3DRect;

    std::string     m_Message;
};
