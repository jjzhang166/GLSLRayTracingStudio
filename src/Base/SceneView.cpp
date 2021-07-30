#include "Base/SceneView.h"

SceneView::SceneView(std::shared_ptr<GLWindow> window, std::shared_ptr<GLScene> scene)
    : m_Window(window)
    , m_Scene(scene)
{

}

SceneView::~SceneView()
{
    m_Scene  = nullptr;
    m_Window = nullptr;
}
