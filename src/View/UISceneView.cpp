#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Common/Log.h"
#include "Math/Math.h"
#include "Base/GLWindow.h"
#include "View/UISceneView.h"
#include "View/Components/LogPanel.h"
#include "Parser/GLTFParser.h"
#include "Misc/FileMisc.h"
#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"

#include <glad/glad.h>

UISceneView::UISceneView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene)
    : SceneView(window, scene)
    , m_ImGuiIO(nullptr)
    , m_MenuBarMousePos(0.0f, 0.0f)
    , m_MenuBarDragging(false)

    , m_ShowingAbout(false)

    , m_PanelProjectWidth(300.0f)
    , m_PanelPropertyWidth(300.0f)
    , m_PanelAssetsWidth(200.0f)
{

}

UISceneView::~UISceneView()
{

}

ImGuiIO& UISceneView::ImIO() const
{
    return ImGui::GetIO();
}

bool UISceneView::Init()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGuiContext* ctx = ImGui::CreateContext();
    if (!ctx)
    {
        return false;
    }

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = "../imgui.ini"; 
    io.LogFilename = "../imgui.log";
    
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(Window()->Window(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float fontScale = WindowsMisc::GetDPI() / 96.0f;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Light.ttf", 16.0f * fontScale);

    m_MenuBarRect.x = 0;
    m_MenuBarRect.y = 20 * fontScale;
    m_MenuBarRect.w = (float)Window()->Width();
    m_MenuBarRect.h = (float)Window()->Height();

    // icons
    m_Icons.Load();

    return true;
}

void UISceneView::Destroy()
{
    m_Icons.Destroy();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UISceneView::OnUpdate()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    UpdatePanelRects();
}

void UISceneView::UpdatePanelRects()
{
    auto rect  = Rectangle2D(0, m_MenuBarRect.h, ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y - m_MenuBarRect.h);
    auto space = ImGui::GetStyle().ItemSpacing;

    // project panel
    m_PanelProjectSize.x  = m_PanelProjectWidth;
    m_PanelProjectSize.y  = rect.h - m_PanelAssetsWidth;

    // property panel
    m_PanelPropertySize.x = m_PanelPropertyWidth;
    m_PanelPropertySize.y = m_PanelProjectSize.y;

    // assets panel
    m_PanelAssetsSize.x   = rect.w;
    m_PanelAssetsSize.y   = m_PanelAssetsWidth;

    // scene 3d
    m_PanelScene3DRect.x = m_PanelProjectSize.x;
    m_PanelScene3DRect.y = m_PanelAssetsSize.y;
    m_PanelScene3DRect.w = rect.w - m_PanelProjectSize.x - m_PanelPropertySize.x;
    m_PanelScene3DRect.h = m_PanelProjectSize.y;
}

void UISceneView::HandleMoving()
{
    if (ImGui::IsMouseDown(0) == false)
    {
        m_MenuBarDragging = false;
    }

    if (m_MenuBarDragging)
    {
        Vector2 currPos = WindowsMisc::GetMousePos();
        Vector2 delta   = currPos - m_MenuBarMousePos;
        if (delta.Size() >= 0.01f)
        {
            m_Window->MoveWindow(delta);
            m_MenuBarMousePos = currPos;
        }
    }
}

void UISceneView::DrawMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        // menu rect
        ImRect rect = ImGui::GetCurrentWindow()->MenuBarRect();
        m_MenuBarRect.Set(rect.Min.x, rect.Min.y, rect.GetWidth(), rect.GetHeight());

        // dragging
        if (ImGui::IsMouseHoveringRect(ImGui::GetCurrentWindow()->MenuBarRect().Min, ImGui::GetCurrentWindow()->MenuBarRect().Max))
        {
            if (ImGui::IsMouseDown(0))
            {
                if (m_MenuBarDragging == false)
                {
                    m_MenuBarDragging = true;
                    m_MenuBarMousePos = WindowsMisc::GetMousePos();
                }
            }
        }

        // File menu items
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open GLTF"))
            {
                std::string fileName = WindowsMisc::OpenFile("GLTF Files\0*.gltf;*.glb\0\0");
                LoadGLTFJob* gltfJob = new LoadGLTFJob(fileName);
                gltfJob->onCompleteEvent = [=](ThreadTask* task) -> void {
                    //m_LogPanel.AddLog("GLTF load complete : %s\n", fileName.c_str());
                };
                JobManager::AddJob(gltfJob);
                //m_LogPanel.AddLog("Loading GLTF : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Open HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                //m_LogPanel.AddLog("Loading HDR file : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Import GLTF"))
            {
                std::string fileName = WindowsMisc::OpenFile("GLTF Files\0*.gltf;*.glb\0\0");
                //m_LogPanel.AddLog("Loading GLTF : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Import HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                //m_LogPanel.AddLog("Loading HDR file : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Test"))
            {
                LOGI("Test\n");
                LOGE("Test2\n");
                LOGW("Test3\n");
            }

            if (ImGui::MenuItem("Quit", "ESC"))
            {
                m_Window->Close();
            }

            ImGui::EndMenu();
        }

        // Help items
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
            {
                m_ShowingAbout = true;
            }

            ImGui::EndMenu();
        }

        // close button
        {
            ImGui::SameLine();

            ImGui::SetCursorPos(ImVec2(m_MenuBarRect.w - 40.0f, m_MenuBarRect.y));
            if (ImGui::Button("X", ImVec2(40.0f, m_MenuBarRect.h)))
            {
                Window()->Close();
            }
        }

        ImGui::EndMenuBar();
    }
}

void UISceneView::DrawAboutUI()
{
    if (m_ShowingAbout)
    {
        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoScrollbar;
        windowFlags |= ImGuiWindowFlags_NoResize;
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoMove;

        ImGui::OpenPopup("About");
        if (ImGui::BeginPopupModal("About", &m_ShowingAbout, windowFlags))
        {
            ImGui::Text("GLSLRayTracingStudio %s", APP_VERSION);
            ImGui::Separator();
            ImGui::Text("By Boblchen contributors.");
            ImGui::Text("GLSLRayTracingStudio is licensed under the MIT License, see LICENSE for more information.");
            ImGui::Text("Github:https://github.com/BobLChen/GLSLRayTracingStudio");
            ImGui::EndPopup();
        }
    }
}

void UISceneView::DrawPropertyPanel()
{
    ImGui::Text("Property");
}

void UISceneView::DrawMessageUI()
{
    if (JobManager::Count() == 0)
    {
        return;
    }

    char bufBegin[32];
    ImFormatString(bufBegin, 32, "Doing %d jobs...", JobManager::Count());
    m_Message = bufBegin;

    static float MsgUIWidth  = 300.0f;
    static float MsgUIHeight = 50.0f;

    ImGui::SetNextWindowSize(ImVec2(MsgUIWidth, MsgUIHeight));
    ImGui::SetNextWindowPos(ImVec2(m_PanelScene3DRect.Left(), m_MenuBarRect.Bottom() + 4));

    ImGui::SetNextWindowBgAlpha(0.75f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 15.0);

    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoDecoration;
    windowFlags |= ImGuiWindowFlags_AlwaysAutoResize;
    windowFlags |= ImGuiWindowFlags_NoSavedSettings;
    windowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoNav;
    windowFlags |= ImGuiWindowFlags_NoMouseInputs;

    if (ImGui::Begin("##notitle", nullptr, windowFlags))
    {
        ImVec2 available = ImGui::GetContentRegionAvail();
        ImVec2 textSize  = ImGui::CalcTextSize(m_Message.c_str(), nullptr, false, available.x);

        ImVec2 pos = ImGui::GetCursorPos();
        pos.x += (available.x - textSize.x) * 0.5f;
        pos.y += (available.y - textSize.y) * 0.5f;

        ImGui::SetCursorPos(pos);
        ImGui::TextWrapped(m_Message.c_str());
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void UISceneView::DrawConsolePanel()
{
    static bool ShowAssetUI = true;

    if (ImGui::Button("Assets"))
    {
        ShowAssetUI = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Console"))
    {
        ShowAssetUI = false;
    }

    if (ShowAssetUI)
    {
        ImGui::Text("Assets");
    }
    else
    {
        Logger().Draw();
    }
}

void UISceneView::DrawProjectPanel()
{
    ImGui::Text("Project");
}

void UISceneView::OnRender()
{
    HandleMoving();

    // main dummy window
    ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    {
        ImGui::SetNextWindowPos(mainViewport->WorkPos);
        ImGui::SetNextWindowSize(mainViewport->WorkSize);
        // All flags to dummy window
        ImGuiWindowFlags hostWindowFlags = 0;
        hostWindowFlags |= ImGuiWindowFlags_NoCollapse;
        hostWindowFlags |= ImGuiWindowFlags_NoResize;
        hostWindowFlags |= ImGuiWindowFlags_NoMove;
        hostWindowFlags |= ImGuiWindowFlags_MenuBar;
        hostWindowFlags |= ImGuiWindowFlags_NoTitleBar;
        hostWindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
        hostWindowFlags |= ImGuiWindowFlags_NoNavFocus;
        hostWindowFlags |= ImGuiWindowFlags_NoScrollbar;
        hostWindowFlags |= ImGuiWindowFlags_NoBackground;
        // Starting dummy window
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("MainDummyWindow", NULL, hostWindowFlags);
        ImGui::PopStyleVar(3);
    }

    // menu bar
    DrawMenuBar();

    // project panel
    {
        ImGui::BeginChild("Project", ImVec2(m_PanelProjectSize.x, m_PanelProjectSize.y), true);
        DrawProjectPanel();
        ImGui::EndChild();
    }

    // dummy scene 3d
    {
        auto space = ImGui::GetStyle().ItemSpacing;
        ImGui::SameLine();
        ImGui::InvisibleButton("ProjectPropertySplitter", ImVec2(m_PanelScene3DRect.w - space.x * 2, m_PanelScene3DRect.h - space.y * 2));
        ImGui::SameLine();
    }

    // property panel
    {
        ImGui::BeginChild("Property", ImVec2(m_PanelPropertySize.x, m_PanelPropertySize.y), true);
        DrawPropertyPanel();
        ImGui::EndChild();
    }

    // assets panel
    {
        ImGuiWindowFlags windowFlags = 0;
        windowFlags |= ImGuiWindowFlags_NoCollapse;
        windowFlags |= ImGuiWindowFlags_NoResize;
        windowFlags |= ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoTitleBar;
        windowFlags |= ImGuiWindowFlags_NoScrollbar;
        ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetMainViewport()->WorkSize.y - m_PanelAssetsWidth));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetMainViewport()->WorkSize.x, m_PanelAssetsWidth));
        ImGui::Begin("Assets&Console", nullptr, windowFlags);
        DrawConsolePanel();
        ImGui::End();
    }

    // end main
    {
        ImGui::End();
    }

    // message
    {
        DrawMessageUI();
    }

    // about
    {
        DrawAboutUI();
    }

    ImGui::ShowDemoWindow();

    {
        ImGui::PushID(0);
        ImVec2 size = ImVec2(32.0f, 32.0f);
        ImVec2 uv0  = ImVec2(0.0f,  0.0f);
        ImVec2 uv1  = ImVec2(1.0f,  1.0f);
        ImVec4 bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
        ImVec4 tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

        ImGui::Image((ImTextureID)(intptr_t)m_Icons.GetIcon(IconName::ICON_CAMERA)->GetTexture(), size, uv0, uv1, tintCol, bgCol);

        ImGui::PopID();
        ImGui::SameLine();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
