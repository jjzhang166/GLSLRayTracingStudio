#pragma once

#include "Core/Scene.h"
#include "Math/Vector2.h"

class UISceneView;

class ProjectPanel
{
public:

    ProjectPanel(UISceneView* uiview, GLScenePtr scene);

    ~ProjectPanel();

    void Draw();

private:

    void DrawNode(Object3DPtr node);

private:

    UISceneView*    m_UIView;
    GLScenePtr      m_Scene;
    Vector2         m_IconSize;
};
