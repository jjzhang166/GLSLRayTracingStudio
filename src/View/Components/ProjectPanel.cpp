#include "View/Icons.h"
#include "View/UISceneView.h"
#include "View/Components/ProjectPanel.h"

#include "imgui.h"
#include "imgui_internal.h"

ProjectPanel::ProjectPanel(UISceneView* uiview, GLScenePtr scene)
    : m_UIView(uiview)
    , m_Scene(scene)
    , m_IconSize(18.0f, 18.0f)
    , m_SelectID(-1)
{
    
}

ProjectPanel::~ProjectPanel()
{

}

// stupid implementation
void ProjectPanel::TreeNode(Object3DPtr node)
{
    IconName iconName = IconName::ICON_NODE;
    if (node->camera)
    {
        iconName = IconName::ICON_CAMERA;
    }
    else if (node->meshes.size() > 0)
    {
        iconName = IconName::ICON_MESH;
    }
    else if (node->light)
    {
        iconName = IconName::ICON_LIGHT;
    }

    bool hovered = false;

    if (node->children.size() > 0)
    {
        if (TreeNode(node->instanceID, node->name.c_str(), iconName))
        {
            for (size_t i = 0; i < node->children.size(); ++i)
            {
                TreeNode(node->children[i]);
            }
            ImGui::TreePop();
        }
    }
    else
    {
        ImGui::Indent();

        // draw node icon
        Icons::DrawImage(iconName, m_IconSize);
        bool hovered = ImGui::IsItemHovered();
        bool clicked = ImGui::IsItemClicked();
        ImVec2 mmin = ImGui::GetItemRectMin();
        ImGui::SameLine();

        // draw node name
        ImGui::Text(node->name.c_str());
        hovered = hovered || ImGui::IsItemHovered();
        clicked = clicked || ImGui::IsItemClicked();
        ImVec2 mmax = ImVec2(ImGui::GetCurrentWindow()->WorkRect.Max.x, ImGui::GetItemRectMax().y);

        // clicked
        if (clicked)
        {
            m_SelectID = node->instanceID;
        }

        // hovered
        if (hovered || m_SelectID == node->instanceID)
        {
            const ImU32 bgCol = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
            ImGui::RenderFrame(mmin, mmax, bgCol, false, 0.0f);
        }

        ImGui::Unindent();
    }
}

// stupid implementation
bool ProjectPanel::TreeNode(int32 id, const char* name, IconName icon)
{
    // tree node
    ImGui::PushID(id);
    bool selected = ImGui::TreeNode("");
    ImGui::PopID();
    bool hovered = ImGui::IsItemHovered();
    bool clicked = ImGui::IsItemClicked();
    ImVec2 mmin = ImGui::GetItemRectMin();
    ImGui::SameLine();

    // draw node icon
    Icons::DrawImage(icon, m_IconSize);
    hovered = hovered || ImGui::IsItemHovered();
    clicked = clicked || ImGui::IsItemClicked();
    ImGui::SameLine();

    // draw node name
    ImGui::Text(name);
    hovered = hovered || ImGui::IsItemHovered();
    clicked = clicked || ImGui::IsItemClicked();
    ImVec2 mmax = ImVec2(ImGui::GetCurrentWindow()->WorkRect.Max.x, ImGui::GetItemRectMax().y);

    // clicked
    if (clicked)
    {
        m_SelectID = id;
    }

    // hovered
    if (hovered || (m_SelectID == id && m_SelectID != -1))
    {
        const ImU32 bgCol = ImGui::GetColorU32(ImGuiCol_HeaderHovered);
        ImGui::RenderFrame(mmin, mmax, bgCol, false, 0.0f);
    }

    return selected;
}

void ProjectPanel::Draw()
{
    ImVec2 txtSize = ImGui::CalcTextSize("A");
    m_IconSize.x   = txtSize.y;
    m_IconSize.y   = txtSize.y;

    Scene3DArray& scenes = m_Scene->GetScenes();

    ImGui::Spacing();
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 4.0f));

    // draw root node
    if (TreeNode(-1, "Scene", IconName::ICON_SCENE))
    {
        for (size_t i = 0; i < scenes.size(); ++i)
        {
            TreeNode(scenes[i]->rootNode);
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
        ImGui::PushID(node->instanceID);
        bool selected = ImGui::TreeNode("");
        ImGui::PopID();

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
