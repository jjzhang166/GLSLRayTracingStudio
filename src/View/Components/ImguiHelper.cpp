#include "View/Components/ImguiHelper.h"

#include "Math/Math.h"

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
    struct Style
    {
        float ctrlPerc    = 0.65f;   // Percentage the value control takes in the pane
        float ctrlMaxSize = 500.f;  // Max pixel size control will take
        float ctrlMinSize = 50.f;   // Minimum size of the control
        float dragMinSize = 150.f;  // Minimum size to be on one line
    };

    static Style style;

    void PropertyLabel(const char* text)
    {
        const int32 Padding = 16;

        const float currentIndent = ImGui::GetCursorPos().x;
        const ImGuiStyle& imstyle = ImGui::GetStyle();
        const ImGuiWindow* window = ImGui::GetCurrentWindow();

        float controlWidth = MMath::Min((ImGui::GetWindowWidth() - imstyle.IndentSpacing) * style.ctrlPerc, style.ctrlMaxSize);
        controlWidth -= window->ScrollbarSizes.x;
        controlWidth  = MMath::Max(controlWidth, style.ctrlMinSize);

        float availableWidth = ImGui::GetContentRegionAvail().x;
        float avaiableTextWidth = availableWidth - controlWidth - Padding;
        ImVec2 textSize = ImGui::CalcTextSize(text, text + strlen(text), false, avaiableTextWidth);
        float indent = currentIndent + availableWidth - controlWidth - textSize.x;

        ImGui::AlignTextToFramePadding();
        ImGui::NewLine();
        ImGui::SameLine(indent);
        ImGui::PushTextWrapPos(indent + avaiableTextWidth);
        ImGui::TextWrapped("%s", text);
        ImGui::PopTextWrapPos();
        ImGui::SameLine();
        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x - Padding);
    }

    bool Combo(const char* label, int32* currentItem, const std::vector<const char*>& items, int32 itemsCount, int32 heightInItems)
    {
        return ImGui::Combo(label, currentItem, [](void* data, int32 idx, const char** outText) { *outText = ((const std::vector<const char*>*)data)->at(idx); return true; }, (void*)&items, itemsCount, heightInItems);
    }
}
