#include "Common/Log.h"

#include "Base/GLWindow.h"
#include "Base/SceneView.h"

#include "Math/Math.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>

static void OnGLFWErrorCallback(int32 error, const char* description)
{
    LOGE("Glfw Error %d: %s\n", error, description);
}

GLWindow::GLWindow(int32 width, int32 height, const char* title, bool resizable)
    : m_Width(width)
    , m_Height(height)
    , m_FrameWidth(width)
    , m_FrameHeight(height)
    , m_Title(title)
    , m_Resizable(resizable)
    , m_Window(nullptr)
    , m_Scene3DView(nullptr)
    , m_UISceneView(nullptr)
    , m_WindowPos(0.0f, 0.0f)
    , m_Closed(false)
{

}

GLWindow::~GLWindow()
{

}

void GLWindow::SetTitle(const char* title)
{
    m_Title = title;
    glfwSetWindowTitle(m_Window, m_Title.c_str());
}

void GLWindow::MoveWindow(Vector2 delta)
{
    m_WindowPos += delta;
    glfwSetWindowPos(m_Window, int32(m_WindowPos.x), int32(m_WindowPos.y));
}

bool GLWindow::Init()
{
    // Setup window callback
    glfwSetErrorCallback(OnGLFWErrorCallback);

    // init glfw
    if (!glfwInit())
    {
        LOGE("glfwInit failed!");
        return false;
    }

    // hint
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    // Create window with graphics context
    m_Window = glfwCreateWindow(Width(), Height(), Title(), NULL, NULL);
    if (!m_Window)
    {
        LOGE("glfwCreateWindow failed!");
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSwapInterval(1);

    // init glad
    if (gladLoadGL() == 0)
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
        m_Window = NULL;
        LOGE("Failed to initialize OpenGL loader!\n");
        return false;
    }

    // get frame size
    glfwGetFramebufferSize(m_Window, &m_FrameWidth, &m_FrameHeight);

    // center window
    SetWindowCenter();

    int32 xPos = 0;
    int32 yPos = 0;
    glfwGetWindowPos(m_Window, &xPos, &yPos);
    m_WindowPos = Vector2((float)xPos, (float)yPos);

    return true;
}

void GLWindow::Destroy()
{
    glfwDestroyWindow(m_Window);
    glfwTerminate();
    m_Window = NULL;
}

bool GLWindow::ShouldClose()
{
    return glfwWindowShouldClose(m_Window) || m_Closed;
}

void GLWindow::Update()
{
    glfwPollEvents();
    m_UISceneView->OnUpdate();
    m_Scene3DView->OnUpdate();
}

void GLWindow::Render()
{
    glfwGetFramebufferSize(m_Window, &m_FrameWidth, &m_FrameHeight);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_SCISSOR_TEST);

    glViewport(0, 0, m_FrameWidth, m_FrameHeight);
    glScissor(0, 0, m_FrameWidth, m_FrameHeight);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_UISceneView->OnRender();
    m_Scene3DView->OnRender();
}

void GLWindow::Present()
{
    glfwSwapBuffers(m_Window);
}

void GLWindow::SetWindowCenter()
{
    int32 sx = 0;
    int32 sy = 0;
    int32 px = 0;
    int32 py = 0;
    int32 mx = 0;
    int32 my = 0;
    int32 monitorCount = 0;
    int32 bestArea = 0;
    int32 finalX = 0;
    int32 finalY = 0;

    glfwGetWindowSize(m_Window, &sx, &sy);
    glfwGetWindowPos(m_Window, &px, &py);

    // Iterate throug all monitors
    GLFWmonitor** monitor = glfwGetMonitors(&monitorCount);
    if (!monitor)
    {
        return;
    }

    for (int32 j = 0; j < monitorCount; ++j)
    {
        glfwGetMonitorPos(monitor[j], &mx, &my);

        const GLFWvidmode* mode = glfwGetVideoMode(monitor[j]);
        if (!mode)
        {
            continue;
        }

        int32 minX = MMath::Max(mx, px);
        int32 minY = MMath::Max(my, py);
        int32 maxX = MMath::Min(mx + mode->width,  px + sx);
        int32 maxY = MMath::Min(my + mode->height, py + sy);
        int32 area = MMath::Max(maxX - minX, 0) * MMath::Max(maxY - minY, 0);

        if (area > bestArea)
        {
            finalX = mx + (mode->width  - sx) / 2;
            finalY = my + (mode->height - sy) / 2;
            bestArea = area;
        }
    }

    if (bestArea)
    {
        glfwSetWindowPos(m_Window, finalX, finalY);
    }
    else
    {
        GLFWmonitor* primary = glfwGetPrimaryMonitor();
        if (primary)
        {
            const GLFWvidmode* desktop = glfwGetVideoMode(primary);
            if (desktop)
            {
                glfwSetWindowPos(m_Window , (desktop->width - sx) / 2 , (desktop->height - sy) / 2);
            }
        }
    }
}
