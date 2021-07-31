#include "View/Icons.h"
#include "View/UISceneView.h"
#include "View/Components/ProjectPanel.h"

#include "imgui.h"

ProjectPanel::ProjectPanel(UISceneView* uiview, GLScenePtr scene)
    : m_UIView(uiview)
    , m_Scene(scene)
    , m_IconSize(18.0f, 18.0f)
{

}

ProjectPanel::~ProjectPanel()
{

}

void ProjectPanel::Draw()
{
    Scene3DArray& scenes = m_Scene->GetScenes();

    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

    bool selected = ImGui::TreeNode("##rootscene");
    ImGui::SameLine();
    Icons::DrawImage(IconName::ICON_CAMERA, Vector2(18.0f, 18.0f));
    ImGui::SameLine();
    ImGui::Text("Scene");

    if (selected)
    {
        for (size_t i = 0; i < scenes.size(); ++i)
        {
            DrawNode(scenes[i]->rootNode);
        }
        ImGui::TreePop();
    }

    ImGui::PopStyleVar(1);
}

void ProjectPanel::DrawNode(Object3DPtr node)
{
    if (node->children.size() > 0)
    {
        // tree node
        std::string labelID = std::string("##") + node->name;
        bool selected = ImGui::TreeNode(labelID.c_str());
        ImGui::SameLine();

        // icon
        if (node->camera != nullptr)
        {
            Icons::DrawImage(IconName::ICON_CAMERA, m_IconSize);
        }
        else if (node->meshes.size() > 0)
        {
            Icons::DrawImage(IconName::ICON_MESH, m_IconSize);
        }
        else if (node->light != nullptr)
        {
            Icons::DrawImage(IconName::ICON_LIGHT, m_IconSize);
        }
        else
        {
            Icons::DrawImage(IconName::ICON_NODE, m_IconSize);
        }
        ImGui::SameLine();

        // name
        ImGui::Text(node->name.c_str());

        // children nodes
        if (selected)
        {
            for (size_t i = 0; i < node->children.size(); ++i)
            {
                DrawNode(node->children[i]);
            }
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Indent();

        // icon
        if (node->camera != nullptr)
        {
            Icons::DrawImage(IconName::ICON_CAMERA, m_IconSize);
        }
        else if (node->meshes.size() > 0)
        {
            Icons::DrawImage(IconName::ICON_MESH, m_IconSize);
        }
        else if (node->light != nullptr)
        {
            Icons::DrawImage(IconName::ICON_LIGHT, m_IconSize);
        }
        else
        {
            Icons::DrawImage(IconName::ICON_NODE, m_IconSize);
        }
        ImGui::SameLine();

        // name
        ImGui::Text(node->name.c_str());

        ImGui::Unindent();
    }
}
