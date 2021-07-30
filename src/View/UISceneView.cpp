#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Common/Log.h"
#include "Math/Math.h"
#include "Base/GLWindow.h"
#include "View/UISceneView.h"
#include "View/Components/LogPanel.h"
#include "Misc/FileMisc.h"
#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"

#include <glad/glad.h>

UISceneView::UISceneView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene)
    : SceneView(window, scene)
    , m_ImGuiIO(nullptr)

    , m_PanelProjectWidth(300.0f)
    , m_PanelPropertyWidth(300.0f)
    , m_PanelAssetsWidth(200.0f)

    , m_MainMenuBar(this)
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

    // set font
    float fontScale = WindowsMisc::GetDPI() / 96.0f;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Light.ttf", 16.0f * fontScale);

    // load icons
    Icons::Init();

    return true;
}

void UISceneView::Destroy()
{
    Icons::Destroy();

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
    Rectangle2D menuBarRect = m_MainMenuBar.GetMenuBarRect();

    auto rect  = Rectangle2D(0, menuBarRect.h, ImGui::GetMainViewport()->WorkSize.x, ImGui::GetMainViewport()->WorkSize.y - menuBarRect.h);
    auto space = ImGui::GetStyle().ItemSpacing;

    // project panel
    m_PanelProjectSize.x = m_PanelProjectWidth;
    m_PanelProjectSize.y = rect.h - m_PanelAssetsWidth;

    // property panel
    m_PanelPropertySize.x = m_PanelPropertyWidth;
    m_PanelPropertySize.y = m_PanelProjectSize.y;

    // assets panel
    m_PanelAssetsSize.x = rect.w;
    m_PanelAssetsSize.y = m_PanelAssetsWidth;

    // scene 3d
    m_PanelScene3DRect.x = m_PanelProjectSize.x;
    m_PanelScene3DRect.y = m_PanelAssetsSize.y;
    m_PanelScene3DRect.w = rect.w - m_PanelProjectSize.x - m_PanelPropertySize.x;
    m_PanelScene3DRect.h = m_PanelProjectSize.y;
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

    Rectangle2D menuBarRect = m_MainMenuBar.GetMenuBarRect();

    char bufBegin[32];
    ImFormatString(bufBegin, 32, "Doing %d jobs...", JobManager::Count());
    m_Message = bufBegin;

    static float MsgUIWidth  = 300.0f;
    static float MsgUIHeight = 50.0f;

    ImGui::SetNextWindowSize(ImVec2(MsgUIWidth, MsgUIHeight));
    ImGui::SetNextWindowPos(ImVec2(m_PanelScene3DRect.Left(), menuBarRect.Bottom() + 4));

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
    m_MainMenuBar.HandleMoving();
    
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
    m_MainMenuBar.Draw();

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

    ImGui::ShowDemoWindow();

    {
        Icons::DrawImage(IconName::ICON_CAMERA);
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
