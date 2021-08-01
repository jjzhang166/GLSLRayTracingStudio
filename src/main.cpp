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
    LOGD("Hello GLSLRayTracingStudio\n");

    // init job manager
    JobManager::Init(8);

    // desk resolution
    int32 deskSizeW = 0;
    int32 deskSizeH = 0;
    WindowsMisc::GetDesktopResolution(deskSizeW, deskSizeH);
    LOGI("Resolution:%dx%d\n", deskSizeW, deskSizeH);

    // make glscene
    auto scene = std::make_shared<GLScene>();
    if (!scene->Init())
    {
        exit(1);
    }

    // make window
    auto window = std::make_shared<GLWindow>(int32(deskSizeW * 0.8f), int32(deskSizeH * 0.8f), "GLSLRayTracingStudio");
    if (!window->Init())
    {
        exit(1);
    }

    // make scene3d view
    auto view3D = std::make_shared<Scene3DView>(window, scene);
    if (!view3D->Init())
    {
        exit(1);
    }

    // make uiscene view
    auto uiView = std::make_shared<UISceneView>(window, scene);
    if (!uiView->Init())
    {
        exit(1);
    }

    // link
    window->SetScene3DView(view3D);
    window->SetUISceneView(uiView);

    // render loop
    while (!window->ShouldClose())
    {
        JobManager::Tick();

        window->Update();
        window->Render();
        window->Present();
    }

    // destroy resources
    scene->Free();
    uiView->Destroy();
    view3D->Destroy();
    window->Destroy();

    // destorey jobmanager
    JobManager::Destroy();
    
    return 0;
}
