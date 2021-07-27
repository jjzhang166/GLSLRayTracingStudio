#include "Common/Log.h"

#include "Base/GLWindow.h"
#include "View/UISceneView.h"
#include "View/Scene3DView.h"
#include "Misc/WindowsMisc.h"
#include "Misc/JobManager.h"
#include "Core/Shader.h"
#include "Core/Scene.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory>

int32 main(int32, char**)
{
    LOGD("Hello GLSLRayTracingStudio");

    JobManager::Init(8);

    int32 deskSizeW = 0;
    int32 deskSizeH = 0;
    WindowsMisc::GetDesktopResolution(deskSizeW, deskSizeH);
    LOGI("Resolution:%dx%d", deskSizeW, deskSizeH);

    auto scene = std::make_shared<GLScene>();

    auto window = std::make_shared<GLWindow>(int32(deskSizeW * 0.8f), int32(deskSizeH * 0.8f), "GLSLRayTracingStudio");
    if (!window->Init())
    {
        exit(1);
    }

    auto view3D = std::make_shared<Scene3DView>(window, scene);
    if (!view3D->Init())
    {
        exit(1);
    }
    
    auto uiView = std::make_shared<UISceneView>(window, scene);
    if (!uiView->Init())
    {
        exit(1);
    }

    window->SetScene3DView(view3D);
    window->SetUISceneView(uiView);

    while (!window->ShouldClose())
    {
        JobManager::Tick();

        window->Update();
        window->Render();
        window->Present();
    }

    uiView->Destroy();
    view3D->Destroy();
    window->Destroy();

    JobManager::Destroy();
    
    return 0;
}
