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

    FORCEINLINE int32 SelectedID() const
    {
        return m_SelectID;
    }

private:

    void DrawNode(Object3DPtr node);

    bool TreeNode(int32 id, const char* name, IconName icon);

    void TreeNode(Object3DPtr node);

private:

    UISceneView*    m_UIView;
    GLScenePtr      m_Scene;
    Vector2         m_IconSize;
    int32           m_SelectID;

};
