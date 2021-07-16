#pragma once

#include "Common/Common.h"

struct WindowsMisc
{
    static void RequestMinimize();
    
    static bool IsThisApplicationForeground();

    static void PreventScreenSaver();

    static void GetDesktopResolution(int32& outWidth, int32& outHeight);

    static float GetDPI();
};
