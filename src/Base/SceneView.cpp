#include "Base/SceneView.h"

SceneView::SceneView(std::shared_ptr<GLWindow> window)
    : m_Window(window)
{

}

SceneView::~SceneView()
{
    m_Window = nullptr;
}
