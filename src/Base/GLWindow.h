#pragma once

#include "Common/Common.h"

#include "Math/Vector2.h"

#include <string>
#include <vector>

struct GLFWwindow;

class SceneView;

class GLWindow
{
public:

    GLWindow() = delete;

    GLWindow(int32 width, int32 height, const char* title, bool resizable);

    ~GLWindow();

    bool Init();

    void Destroy();

    bool ShouldClose();

    void Update();

    void Render();

    void Present();

    void SetTitle(const char* title);

    void MoveWindow(Vector2 delta);

    FORCEINLINE void AddView(std::shared_ptr<SceneView> view)
    {
        auto it = std::find(m_Views.begin(), m_Views.end(), view);
        if (it == m_Views.end())
        {
            m_Views.push_back(view);
        }
    }

    FORCEINLINE void RemoveView(std::shared_ptr<SceneView> view)
    {
        auto it = std::find(m_Views.begin(), m_Views.end(), view);
        if (it != m_Views.end())
        {
            m_Views.erase(it);
        }
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

    FORCEINLINE bool Resizable() const
    {
        return m_Resizable;
    }

    FORCEINLINE GLFWwindow* Window() const
    {
        return m_Window;
    }

private:

    void SetWindowCenter();

private:

    typedef std::vector<std::shared_ptr<SceneView>> SceneViewArray;

    int32               m_Width;
    int32               m_Height;
    int32               m_FrameWidth;
    int32               m_FrameHeight;
    std::string         m_Title;
    bool                m_Resizable;
    GLFWwindow*         m_Window;
    SceneViewArray      m_Views;
    Vector2             m_WindowPos;
    bool                m_Closed;
};
