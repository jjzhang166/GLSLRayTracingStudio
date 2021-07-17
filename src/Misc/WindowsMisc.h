#pragma once

#include "Common/Common.h"

#include "Math/Vector2.h"

#include <string>

struct WindowsMisc
{
    static void RequestMinimize();
    
    static bool IsThisApplicationForeground();

    static void PreventScreenSaver();

    static void GetDesktopResolution(int32& outWidth, int32& outHeight);

    static float GetDPI();

    static std::string OpenFile(const char* filter);

    static Vector2 GetMousePos();
};
