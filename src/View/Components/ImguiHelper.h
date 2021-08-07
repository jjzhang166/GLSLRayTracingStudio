#pragma once

#include "Common/Common.h"

#include <string>
#include <vector>

namespace ImGui
{
    void PropertyLabel(const char* text);

    bool Combo(const char* label, int32* currentItem, const std::vector<const char*>& items, int32 itemsCount, int32 heightInItems = -1);
}
