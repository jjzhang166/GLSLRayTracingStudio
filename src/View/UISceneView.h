#pragma once

#include "Base/SceneView.h"
#include "Math/Vector2.h"
#include "Math/Rectangle2D.h"

#include "View/Icons.h"
#include "View/Components/LogPanel.h"
#include "View/Components/MainMenuBar.h"
#include "View/Components/ProjectPanel.h"
#include "View/Components/PropertyPanel.h"

#include <memory>

struct ImGuiIO;
class GLWindow;

class UISceneView : public SceneView
{
public:

    UISceneView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene);

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

    void DrawConsolePanel();

    void DrawMessageUI();

private:

    ImGuiIO*        m_ImGuiIO;

    float           m_PanelProjectWidth;
    float           m_PanelPropertyWidth;
    float           m_PanelAssetsWidth;

    Vector2         m_PanelProjectSize;
    Vector2         m_PanelPropertySize;
    Vector2         m_PanelAssetsSize;
    Rectangle2D     m_PanelScene3DRect;

    MainMenuBar     m_MainMenuBar;
    ProjectPanel    m_ProjectPanel;
    PropertyPanel   m_PropertyPanel;

    std::string     m_Message;
};
