#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Common/Log.h"
#include "Math/Math.h"
#include "Base/GLWindow.h"
#include "View/UISceneView.h"
#include "Parser/GLTFParser.h"
#include "Misc/FileMisc.h"
#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"

const static float TitleBarHeight = 19;

UISceneView::UISceneView(std::shared_ptr<GLWindow> window)
    : SceneView(window)
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
    //io.IniFilename = "../imgui.ini"; 
    //io.LogFilename = "../imgui.log";
    
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(Window()->Window(), true);
    ImGui_ImplOpenGL3_Init("#version 130");

    float fontScale = WindowsMisc::GetDPI() / 96.0f;
    io.Fonts->AddFontFromFileTTF("../assets/fonts/Roboto-Light.ttf", 16.0f * fontScale);

    return true;
}

void UISceneView::Destroy()
{
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
    const ImGuiViewport* mainViewport = ImGui::GetMainViewport();
    const Rectangle2D mainRect(mainViewport->WorkPos.x, mainViewport->WorkPos.y, mainViewport->WorkSize.x, mainViewport->WorkSize.y);

    m_PanelProjectRect.Set(mainRect.x, mainRect.y, m_PanelProjectWidth, mainRect.h - m_PanelAssetsWidth - TitleBarHeight);
    m_PanelPropertyRect.Set(mainRect.w - m_PanelPropertyWidth, mainRect.y, m_PanelPropertyWidth, mainRect.h - m_PanelAssetsWidth - TitleBarHeight);
    m_PanelAssetsRect.Set(mainRect.x, mainRect.h - m_PanelAssetsWidth, mainRect.w, m_PanelAssetsWidth);
    m_PanelScene3DRect.Set(m_PanelProjectRect.Right(), mainRect.h - m_PanelProjectRect.h - TitleBarHeight, mainRect.w - m_PanelPropertyWidth - m_PanelProjectWidth, m_PanelProjectRect.h);
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
    // TODO:find a way to show close button.
    if (ImGui::BeginMainMenuBar())
    {
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
                    LOGD("GLTF load complete : %s", fileName.c_str());
                };
                JobManager::AddJob(gltfJob);
                LOGD("Loading GLTF : %s", fileName.c_str());
            }

            if (ImGui::MenuItem("Open HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                LOGD("HDR file : %s", fileName.c_str());
            }

            if (ImGui::MenuItem("Import GLTF"))
            {
                std::string fileName = WindowsMisc::OpenFile("GLTF Files\0*.gltf;*.glb\0\0");
                LOGD("GLTF file : %s", fileName.c_str());
            }

            if (ImGui::MenuItem("Import HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                LOGD("HDR file : %s", fileName.c_str());
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
    }

    ImGui::EndMainMenuBar();
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
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(m_PanelPropertyRect.x, m_PanelPropertyRect.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(m_PanelPropertyRect.w, m_PanelPropertyRect.h), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Property", nullptr, windowFlags);
    ImGui::PopStyleVar(3);

    ImGui::End();
}

void UISceneView::DrawMessageUI()
{
    if (JobManager::Count() == 0)
    {
        return;
    }

    char buf[32];
    ImFormatString(buf, 32, "Doing %d jobs...", JobManager::Count());
    m_Message = buf;

    static float MsgUIWidth  = 300.0f;
    static float MsgUIHeight = 50.0f;

    ImGui::SetNextWindowSize(ImVec2(MsgUIWidth, MsgUIHeight));
    ImGui::SetNextWindowPos(ImVec2(m_PanelScene3DRect.Left(), m_PanelScene3DRect.Bottom() - MsgUIHeight + TitleBarHeight));

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

void UISceneView::DrawAssetsPanel()
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(m_PanelAssetsRect.x, m_PanelAssetsRect.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(m_PanelAssetsRect.w, m_PanelAssetsRect.h), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Assets", nullptr, windowFlags);
    ImGui::PopStyleVar(3);


    ImGui::End();
}

void UISceneView::DrawProjectPanel()
{
    ImGuiWindowFlags windowFlags = 0;
    windowFlags |= ImGuiWindowFlags_NoMove;
    windowFlags |= ImGuiWindowFlags_NoCollapse;
    windowFlags |= ImGuiWindowFlags_NoResize;
    windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::SetNextWindowPos(ImVec2(m_PanelProjectRect.x, m_PanelProjectRect.y), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(m_PanelProjectRect.w, m_PanelProjectRect.h), ImGuiCond_FirstUseEver);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Project", nullptr, windowFlags);
    ImGui::PopStyleVar(3);


    ImGui::End();
}

void UISceneView::OnRender()
{
    HandleMoving();

    DrawAboutUI();

    DrawMenuBar();

    DrawProjectPanel();

    DrawPropertyPanel();

    DrawAssetsPanel();

    DrawMessageUI();

    //ImGui::ShowDemoWindow();
    
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
