#pragma once

#include "Common/Common.h"

#include "Math/Vector2.h"
#include "View/Scene3DView.h"
#include "View/UISceneView.h"

#include <string>
#include <vector>

struct GLFWwindow;

class GLWindow
{
public:

    GLWindow() = delete;

    GLWindow(int32 width, int32 height, const char* title);

    ~GLWindow();

    bool Init();

    void Destroy();

    bool ShouldClose();

    void Update();

    void Render();

    void Present();

    void SetTitle(const char* title);

    void MoveWindow(Vector2 delta);

    FORCEINLINE void SetScene3DView(std::shared_ptr<Scene3DView> view)
    {
        m_Scene3DView = view;
    }

    FORCEINLINE std::shared_ptr<Scene3DView> GetScene3DView() const
    {
        return m_Scene3DView;
    }

    FORCEINLINE void SetUISceneView(std::shared_ptr<UISceneView> view)
    {
        m_UISceneView = view;
    }

    FORCEINLINE std::shared_ptr<UISceneView> GetUISceneView() const
    {
        return m_UISceneView;
    }

    FORCEINLINE void Close()
    {
        m_Closed = true;
    }

    FORCEINLINE int32 Width() const
    {
        return m_Width;
    }

    FORCEINLINE int32 Height() const
    {
        return m_Height;
    }

    FORCEINLINE const char* Title() const
    {
        return m_Title.c_str();
    }

    FORCEINLINE int32 FrameWidth() const
    {
        return m_FrameWidth;
    }

    FORCEINLINE int32 FrameHeight() const
    {
        return m_FrameHeight;
    }

    FORCEINLINE GLFWwindow* Window() const
    {
        return m_Window;
    }

private:

    void SetWindowCenter();

private:

    int32                           m_Width;
    int32                           m_Height;
    int32                           m_FrameWidth;
    int32                           m_FrameHeight;
    std::string                     m_Title;
    GLFWwindow*                     m_Window;
    std::shared_ptr<Scene3DView>    m_Scene3DView;
    std::shared_ptr<UISceneView>    m_UISceneView;
    Vector2                         m_WindowPos;
    bool                            m_Closed;
};
