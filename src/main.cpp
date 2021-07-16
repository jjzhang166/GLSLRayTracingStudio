#include "Common/Log.h"

#include "Base/GLWindow.h"
#include "UI/UISceneView.h"
#include "Misc/WindowsMisc.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory>

int main(int, char**)
{
    LOGD("Hello GLSLRayTracingStudio");

    int32 deskSizeW = 0;
    int32 deskSizeH = 0;
    WindowsMisc::GetDesktopResolution(deskSizeW, deskSizeH);
    LOGI("Resolution:%dx%d", deskSizeW, deskSizeH);

    auto window = std::make_shared<GLWindow>(1280, 720, "GLSLRayTracingStudio", true);
    if (!window->Init())
    {
        exit(1);
    }

    auto uiView = std::make_shared<UISceneView>(window);
    if (!uiView->Init())
    {
        exit(1);
    }

    window->AddView(uiView);

    while (!window->ShouldClose())
    {
        window->Update();

        window->Render();

        window->Present();
    }

    uiView->Destroy();
    window->Destroy();
    
    return 0;
}
