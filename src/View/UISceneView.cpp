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

#include <glad/glad.h>

struct LogHelper
{
    ImGuiTextBuffer     textBuffer;
    ImVector<int>       lineOffsets;

    LogHelper()
    {
        Clear();
    }

    void Clear()
    {
        textBuffer.clear();
        lineOffsets.clear();
        lineOffsets.push_back(0);
    }

    void AddLog(const char* fmt, ...)
    {
        int oldSize = textBuffer.size();

        va_list args;
        va_start(args, fmt);
        textBuffer.appendfv(fmt, args);
        va_end(args);

        for (int newSize = textBuffer.size(); oldSize < newSize; oldSize++)
        {
            if (textBuffer[oldSize] == '\n')
            {
                lineOffsets.push_back(oldSize + 1);
            }
        }
    }

    void Draw()
    {
        ImGui::BeginChild("LogScrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));

        const char* bufBegin = textBuffer.begin();
        const char* bufEnd   = textBuffer.end();

        ImGuiListClipper clipper;
        clipper.Begin(lineOffsets.Size);
        while (clipper.Step())
        {
            for (int lineNo = clipper.DisplayStart; lineNo < clipper.DisplayEnd; lineNo++)
            {
                const char* lineStart = bufBegin + lineOffsets[lineNo];
                const char* lineEnd   = (lineNo + 1 < lineOffsets.Size) ? (bufBegin + lineOffsets[lineNo + 1] - 1) : bufEnd;

                ImVec4 color;
                bool hasColor = false;
                if (strstr(lineStart, "[error]"))
                {
                    color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f);
                    hasColor = true;
                }
                else if (strstr(lineStart, "[warning]"))
                {
                    color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f);
                    hasColor = true;
                }

                if (hasColor)
                {
                    ImGui::PushStyleColor(ImGuiCol_Text, color);
                }

                ImGui::TextUnformatted(lineStart, lineEnd);

                if (hasColor)
                {
                    ImGui::PopStyleColor();
                }
            }
        }
        clipper.End();

        ImGui::PopStyleVar();

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
        {
            ImGui::SetScrollHereY(1.0f);
        }

        ImGui::EndChild();
    }
};

static LogHelper logHelper;

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

    m_MenuBarRect.x = 0;
    m_MenuBarRect.y = 20 * fontScale;
    m_MenuBarRect.w = (float)Window()->Width();
    m_MenuBarRect.h = (float)Window()->Height();

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
    m_PanelScene3DRect.h = m_PanelProjectSize.y + space.y;
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
                    LOGD("GLTF load complete : %s", fileName.c_str());
                };
                JobManager::AddJob(gltfJob);
                LOGD("Loading GLTF : %s", fileName.c_str());
            }

            if (ImGui::MenuItem("Open HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                LOGD("HDR file : %s", fileName.c_str());
                logHelper.AddLog("[error] something went wrong");
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

void UISceneView::DrawAssetsPanel()
{
    if (ImGui::BeginTabBar("##Tabs", ImGuiTabBarFlags_None))
    {
        if (ImGui::BeginTabItem("Assets"))
        {
            ImGui::Text("Assets");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Console"))
        {
            logHelper.Draw();
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
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
        ImGui::BeginChild("Assets", ImVec2(m_PanelAssetsSize.x, m_PanelAssetsSize.y), true);
        DrawAssetsPanel();
        ImGui::EndChild();
    }

    // end main
    {
        ImGui::End();
    }

    // other panel
    DrawMessageUI();
    DrawAboutUI();

    ImGui::ShowDemoWindow();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
