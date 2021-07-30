#include "Common/Log.h"

#include "Base/GLWindow.h"

#include "View/UISceneView.h"
#include "View/Components/MainMenuBar.h"

#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"

#include "Parser/GLTFParser.h"

#include "imgui.h"
#include "imgui_internal.h"

MainMenuBar::MainMenuBar(UISceneView* uiview)
    : m_UIView(uiview)
{
    float fontScale = WindowsMisc::GetDPI() / 96.0f;

    m_MenuBarRect.x = 0;
    m_MenuBarRect.y = 20 * fontScale;
    m_MenuBarRect.w = (float)m_UIView->Window()->Width();
    m_MenuBarRect.h = (float)m_UIView->Window()->Height();
}

MainMenuBar::~MainMenuBar()
{
    m_UIView = nullptr;
}

void MainMenuBar::HandleMoving()
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
            m_UIView->Window()->MoveWindow(delta);
            m_MenuBarMousePos = currPos;
        }
    }
}

void MainMenuBar::Draw()
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
                    LOGI("GLTF load complete : %s\n", fileName.c_str());
                };
                JobManager::AddJob(gltfJob);
                LOGI("Loading GLTF : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Open HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                LOGI("Loading HDR file : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Import GLTF"))
            {
                std::string fileName = WindowsMisc::OpenFile("GLTF Files\0*.gltf;*.glb\0\0");
                LOGI("Loading GLTF : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Import HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                LOGI("Loading HDR file : %s\n", fileName.c_str());
            }

            if (ImGui::MenuItem("Test"))
            {
                LOGI("Test\n");
                LOGE("Test2\n");
                LOGW("Test3\n");
            }

            if (ImGui::MenuItem("Quit", "ESC"))
            {
                m_UIView->Window()->Close();
            }
            ImGui::EndMenu();
        }
        
        // Help items
        if (ImGui::BeginMenu("Help"))
        {
            if (ImGui::MenuItem("About"))
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
            ImGui::EndMenu();
        }

        // close button
        {
            ImGui::SameLine();

            ImGui::SetCursorPos(ImVec2(m_MenuBarRect.w - 40.0f, m_MenuBarRect.y));
            if (ImGui::Button("X", ImVec2(40.0f, m_MenuBarRect.h)))
            {
                m_UIView->Window()->Close();
            }
        }

        ImGui::EndMenuBar();
    }
}
