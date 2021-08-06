#include "Base/GLWindow.h"
#include "View/Scene3DView.h"
#include "Parser/HDRParser.h"
#include "Misc/FileMisc.h"
#include "Renderer/PBRRenderer.h"
#include "Renderer/RayTracingRenderer.h"

#include <glad/glad.h>

Scene3DView::Scene3DView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene)
    : SceneView(window, scene)
{

}

Scene3DView::~Scene3DView()
{

}

bool Scene3DView::Init()
{
    m_PBRRenderer = std::make_shared<PBRRenderer>();
    m_RayRenderer = std::make_shared<RayTracingRenderer>();

    m_PBRRenderer->Init();
    m_RayRenderer->Init();

    m_PBRRenderer->SetScene(m_Scene);
    m_RayRenderer->SetScene(m_Scene);

    // default hdr
    LoadHDRJob hdrParser(GetRootPath() + "assets/env/output_skybox.hdr");
    hdrParser.DoThreadedWork();
    m_Scene->AddHDR(hdrParser.GetHDRImage());

    return true;
}

void Scene3DView::Destroy()
{
    m_PBRRenderer->Destroy();
    m_RayRenderer->Destroy();

    m_PBRRenderer = nullptr;
    m_RayRenderer = nullptr;
}

void Scene3DView::OnUpdate()
{
    static double lastTime = ImGui::GetTime();
    double currTime = ImGui::GetTime();
    double deltaTime = currTime - lastTime;
    lastTime = currTime;

    auto camera  = m_Scene->GetCamera();
    auto rect    = Window()->GetUISceneView()->ViewPort3D();
    ImVec2 mouse = ImGui::GetMousePos();

    if (!rect.Contains(mouse.x, mouse.y))
    {
        camera->OnMousePos(Vector2(mouse.x, mouse.y));
        camera->OnRMouse(false);
	    camera->OnMMouse(false);
	    camera->OnMouseWheel(0.0f);
        camera->Update((float)deltaTime);
    }
    else
    {
        camera->OnMousePos(Vector2(mouse.x, mouse.y));
        camera->OnRMouse(ImGui::IsMouseDown(1));
	    camera->OnMMouse(ImGui::IsMouseDown(2));
	    camera->OnMouseWheel(ImGui::GetIO().MouseWheel);
        camera->Update((float)deltaTime);
    }
}

void Scene3DView::OnRender()
{
    auto rect = Window()->GetUISceneView()->ViewPort3D();
    GLint tx = (GLint)rect.x;
    GLint ty = (GLint)rect.y;
    GLsizei tw = (GLsizei)rect.w;
    GLsizei th = (GLsizei)rect.h;

    // blend
    glDisable(GL_BLEND);
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
    glBlendFuncSeparate(GL_SRC_COLOR, GL_DST_COLOR, GL_SRC_ALPHA, GL_DST_ALPHA);

    // depth
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // stencil
    glDisable(GL_STENCIL_TEST);
    glStencilFuncSeparate(GL_FRONT_AND_BACK, GL_ALWAYS, 0, 255);
    glStencilOpSeparate(GL_FRONT_AND_BACK, GL_KEEP, GL_KEEP, GL_KEEP);

    // cull face
    glDisable(GL_CULL_FACE);
    glCullFace(GL_NONE);
    
    glViewport(tx, ty, tw, th);
    glScissor(tx, ty, tw, th);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_PBRRenderer->Render();
}
