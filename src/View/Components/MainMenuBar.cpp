#include "Common/Log.h"

#include "Base/GLWindow.h"

#include "View/UISceneView.h"
#include "View/Components/MainMenuBar.h"

#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"

#include "Parser/GLTFParser.h"
#include "Parser/HDRParser.h"

#include "Renderer/IBLSampler.h"

#include "imgui.h"
#include "imgui_internal.h"

MainMenuBar::MainMenuBar(UISceneView* uiview, GLScenePtr scene)
    : m_UIView(uiview)
    , m_Scene(scene)
    , m_ShowingAbout(false)
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
                if (!fileName.empty())
                {
                    m_Scene->Free();

                    LoadGLTFJob* gltfJob = new LoadGLTFJob(fileName);
                    gltfJob->onCompleteEvent = [=](ThreadTask* task) -> void
                    {
                        LOGI("GLTF load complete : %s\n", fileName.c_str());
                        m_Scene->AddScene(gltfJob->GetScene());
                        m_Scene->GetCamera()->SetAspect(m_UIView->Window()->FrameWidth() * 1.0f / m_UIView->Window()->FrameHeight());
                        m_Scene->Build();
                    };
                    
                    JobManager::AddJob(gltfJob);
                    LOGI("Loading GLTF : %s\n", fileName.c_str());
                }
            }

            if (ImGui::MenuItem("Import GLTF"))
            {
                std::string fileName = WindowsMisc::OpenFile("GLTF Files\0*.gltf;*.glb\0\0");
                if (!fileName.empty())
                {
                    LoadGLTFJob* gltfJob = new LoadGLTFJob(fileName);
                    gltfJob->onCompleteEvent = [=](ThreadTask* task) -> void
                    {
                        LOGI("GLTF load complete : %s\n", fileName.c_str());
                        m_Scene->AddScene(gltfJob->GetScene());
                        m_Scene->GetCamera()->SetAspect(m_UIView->Window()->FrameWidth() * 1.0f / m_UIView->Window()->FrameHeight());
                        m_Scene->Build();
                    };

                    JobManager::AddJob(gltfJob);
                    LOGI("Loading GLTF : %s\n", fileName.c_str());
                }
            }

            if (ImGui::MenuItem("Import HDR"))
            {
                std::string fileName = WindowsMisc::OpenFile("HDR Files\0*.hdr\0\0");
                if (!fileName.empty())
                {
                    LoadHDRJob* hdrJob = new LoadHDRJob(fileName);
                    hdrJob->onCompleteEvent = [=](ThreadTask* task) -> void
                    {
                        LOGI("HDR load complete : %s\n", fileName.c_str());
                        m_Scene->AddHDR(hdrJob->GetHDRImage());

                        IBLSampler sampler;
                        sampler.Init(hdrJob->GetHDRImage());
                    };

                    JobManager::AddJob(hdrJob);
                    LOGI("Loading HDR file : %s\n", fileName.c_str());
                }
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
                m_UIView->Window()->Close();
            }
        }

        ImGui::EndMenuBar();
    }
}
