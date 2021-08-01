#pragma once

#include "Core/Scene.h"
#include "Math/Vector2.h"

class UISceneView;

class PropertyPanel
{
public:

    PropertyPanel(UISceneView* uiview, GLScenePtr scene);

    ~PropertyPanel();

    void Draw(int32 instanceID);

private:

    Object3DPtr FindObject(int32 id);

    void DrawSceneSettings();

    void DrawPropertyTransform(Object3DPtr node);

    void DrawPropertyCamera(CameraPtr camera);

    void DrawPropertyLight(LightPtr light);

    void DrawPropertyMesh(const MeshArray& meshes);

private:

    UISceneView*    m_UIView;
    GLScenePtr      m_Scene;

};
