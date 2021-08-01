#include "View/Components/PropertyPanel.h"
#include "View/Components/ImguiHelper.h"

#include "Math/Math.h"

#include "imgui.h"
#include "imgui_internal.h"

PropertyPanel::PropertyPanel(UISceneView* uiview, GLScenePtr scene)
    : m_UIView(uiview)
    , m_Scene(scene)
{

}

PropertyPanel::~PropertyPanel()
{

}

Object3DPtr PropertyPanel::FindObject(int32 id)
{
    Scene3DArray& scenes = m_Scene->GetScenes();
    for (size_t sceneID = 0; sceneID < scenes.size(); ++sceneID)
    {
        Object3DArray& nodes = scenes[sceneID]->nodes;
        for (size_t nodeID = 0; nodeID < nodes.size(); ++nodeID)
        {
            if (nodes[nodeID]->instanceID == id)
            {
                return nodes[nodeID];
            }
        }
    }

    return nullptr;
}

void PropertyPanel::DrawSceneSettings()
{
    static bool settingsOpend = true;
    ImGui::SetNextTreeNodeOpen(settingsOpend);
    settingsOpend = ImGui::CollapsingHeader("Settings");

    if (settingsOpend)
    {
        {
            static int maxDepth = 4;
            ImGui::PropertyLabel("Max Depth");
            ImGui::SameLine();
            ImGui::SliderInt("##SettingsMaxDepth", &maxDepth, 4, 32);
        }

        {
            static int numTilesX = 4;
            ImGui::PropertyLabel("NumTimesX");
            ImGui::SameLine();
            ImGui::SliderInt("##SettingsNumTilesX", &numTilesX, 4, 64);
        }

        {
            static int numTilesY = 4;
            ImGui::PropertyLabel("NumTimesY");
            ImGui::SameLine();
            ImGui::SliderInt("##SettingsNumTilesY", &numTilesY, 4, 64);
        }
    }
}

void PropertyPanel::DrawPropertyTransform(Object3DPtr node)
{
    static bool transformOpend = true;
    ImGui::SetNextTreeNodeOpen(transformOpend);
    transformOpend = ImGui::CollapsingHeader("Transform");

    if (transformOpend)
    {
        Matrix4x4& transform = node->transform;

        Vector4 pos;
        Vector4 sca;
        Vector4 rot;
        transform.Decompose(Matrix4x4::Style::EulerAngles, pos, sca, rot);

        // position
        {
            ImGui::PropertyLabel("Position");
            ImGui::SameLine();
            ImGui::DragFloat3("##TransformPosition", (float*)&pos);
        }

        // rotation
        {
            ImGui::PropertyLabel("Rotation");
            ImGui::SameLine();
            ImGui::DragFloat3("##TransformRotation", (float*)&rot);
        }

        // scale
        {
            ImGui::PropertyLabel("Scale");
            ImGui::SameLine();
            ImGui::DragFloat3("##TransformScale", (float*)&sca);
        }

        transform.Recompose(pos, sca, rot);
    }
}

void PropertyPanel::DrawPropertyLight(LightPtr light)
{
    static bool lightOpend = true;
    ImGui::SetNextTreeNodeOpen(lightOpend);
    lightOpend = ImGui::CollapsingHeader("Light");

    if (lightOpend)
    {
        // color
        {
            ImGui::PropertyLabel("Color");
            ImGui::SameLine();
            ImGui::ColorEdit3("##LightColor", (float*)&(light->color));
        }

        // Intensity
        {
            ImGui::PropertyLabel("Intensity");
            ImGui::SameLine();
            ImGui::DragFloat("##LightIntensity", &(light->intensity), 0.05f, 0.0f, 10.0f);
        }

        // type
        {
            ImGui::PropertyLabel("Type");
            ImGui::SameLine();
            const char* items[] = { "DIRECTIONAL", "POINT", "SPOT" };
            ImGui::Combo("##LightType", &(light->type), items, IM_ARRAYSIZE(items));
        }

        if (light->type == Light::LightType::DIRECTIONAL)
        {
            // direction
            {
                Vector3 direction = light->node->GlobalTransform().GetForward();
                ImGui::PropertyLabel("Direction");
                ImGui::SameLine();
                ImGui::DragFloat3("##LightDirection", (float*)&direction, 0.0f, 0.0f, 0.0f);
            }
        }
        else if (light->type == Light::LightType::POINT)
        {
            // Position
            {
                Vector3 position = light->node->GlobalTransform().GetOrigin();
                ImGui::PropertyLabel("Position");
                ImGui::SameLine();
                ImGui::DragFloat3("##LightPosition", (float*)&position, 0.0f, 0.0f, 0.0f);
            }

            // Range
            {
                ImGui::PropertyLabel("Range");
                ImGui::SameLine();
                ImGui::DragFloat("##LightRange", &(light->range), 1.0f, 0.0f, 1000.0f);
            }
        }
        else if (light->type == Light::LightType::SPOT)
        {
            // Position
            {
                Vector3 position = light->node->GlobalTransform().GetOrigin();
                ImGui::PropertyLabel("Position");
                ImGui::SameLine();
                ImGui::DragFloat3("##LightPosition", (float*)&position, 0.0f, 0.0f, 0.0f);
            }

            // Range
            {
                ImGui::PropertyLabel("Range");
                ImGui::SameLine();
                ImGui::DragFloat("##LightRange", &(light->range), 1.0f, 0.0f, 1000.0f);
            }

            // Inner Cone
            {
                ImGui::PropertyLabel("Inner Cone");
                ImGui::SameLine();
                ImGui::DragFloat("##LightInnerCone", &(light->innerCone), 1.0f, 0.0f, 180.0f);
            }

            // Outer Cone
            {
                ImGui::PropertyLabel("Outer Cone");
                ImGui::SameLine();
                ImGui::DragFloat("##LightOuterCone", &(light->outerCone), 1.0f, 0.0f, 180.0f);
            }
        }

    }
}

void PropertyPanel::DrawPropertyCamera(CameraPtr camera)
{
    static bool cameraOpend = true;
    ImGui::SetNextTreeNodeOpen(cameraOpend);
    cameraOpend = ImGui::CollapsingHeader("Camera");

    if (cameraOpend)
    {
        float zNear  = camera->GetNear();
        float zFar   = camera->GetFar();
        float fov    = MMath::RadiansToDegrees(camera->GetFov());
        float aspect = camera->GetAspect();
        
        // near
        {
            ImGui::PropertyLabel("Near");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraNear", &zNear, 1.0f, 0.01f, 100.0f);
        }

        // far
        {
            ImGui::PropertyLabel("Far");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraFar", &zFar, 10.0f, 100.0f, 30000.0f);
        }

        // fov
        {
            ImGui::PropertyLabel("Fov");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraFov", &fov, 1.0f, 1.0f, 89.0f);
        }

        // aspect
        {
            ImGui::PropertyLabel("Aspect");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraAspect", &aspect, 0.0f, aspect, aspect);
        }

        // focalDist
        {
            ImGui::PropertyLabel("Focal Distance");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraFocalDistance", &camera->focalDist);
        }

        // aperture
        {
            ImGui::PropertyLabel("Aperture");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraAperture", &camera->aperture);
        }

        // smooth
        {
            ImGui::PropertyLabel("Smooth");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraSmooth", &camera->smooth);
        }

        // Speed
        {
            ImGui::PropertyLabel("Speed");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraSpeed", &camera->speed, 0.0f, 0.1f, 5.0f);
        }

        // SpeedFactor
        {
            ImGui::PropertyLabel("SpeedFactor");
            ImGui::SameLine();
            ImGui::DragFloat("##CameraSpeedFactor", &camera->speedFactor, 0.0f, 0.1f, 5.0f);
        }

        camera->Perspective(MMath::DegreesToRadians(fov), aspect, zNear, zFar);
    }
}

void PropertyPanel::DrawPropertyMesh(const MeshArray& meshes)
{
    static bool meshesOpend = true;
    ImGui::SetNextTreeNodeOpen(meshesOpend);
    meshesOpend = ImGui::CollapsingHeader("Mesh");
    
    if (meshesOpend)
    {
        if (meshes.size() == 1)
        {
            // vertex count
            {
                int32 vertexCount = (int32)(meshes[0]->positions.size());
                ImGui::PropertyLabel("Vertex Count");
                ImGui::SameLine();
                ImGui::DragInt("##MeshVertexCount", &vertexCount, 0.0f, vertexCount, vertexCount);
            }

            // triangles
            {
                int32 faceCount = (int32)(meshes[0]->indices.size()) / 3;
                ImGui::PropertyLabel("Face Count");
                ImGui::SameLine();
                ImGui::DragInt("##MeshFaceCount", &faceCount, 0.0f, faceCount, faceCount);
            }
        }
        else
        {
            int32 totalVertexCount = 0;
            int32 totalFaceCount   = 0;
            int32 subMeshCount     = (int32)(meshes.size());
            for (int32 subMesh = 0; subMesh < subMeshCount; ++subMesh)
            {
                int32 vertexCount = (int32)(meshes[subMesh]->positions.size());
                int32 indexCount  = (int32)(meshes[subMesh]->indices.size());
                totalVertexCount += vertexCount;
                totalFaceCount   += indexCount / 3;
            }

            // SubMeshCount
            {
                ImGui::PropertyLabel("SubMesh Count");
                ImGui::SameLine();
                ImGui::DragInt("##MeshSubMeshCount", &subMeshCount, 0.0f, subMeshCount, subMeshCount);
            }

            // TotalVertexCount
            {
                ImGui::PropertyLabel("Vertex Count");
                ImGui::SameLine();
                ImGui::DragInt("##MeshTotalVertexCount", &totalVertexCount, 0.0f, totalVertexCount, totalVertexCount);
            }

            // TotalIndexCount
            {
                ImGui::PropertyLabel("Face Count");
                ImGui::SameLine();
                ImGui::DragInt("##MeshTotalFaceCount", &totalFaceCount, 0.0f, totalFaceCount, totalFaceCount);
            }

        }
    }
}

void PropertyPanel::Draw(int32 instanceID)
{
    if (instanceID == -1)
    {
        DrawSceneSettings();
        return;
    }

    Object3DPtr node = FindObject(instanceID);
    if (node == nullptr)
    {
        DrawSceneSettings();
        return;
    }

    // transform
    DrawPropertyTransform(node);

    // camera
    if (node->camera)
    {
        DrawPropertyCamera(node->camera);
    }

    // light
    if (node->light)
    {
        DrawPropertyLight(node->light);
    }

    // mesh
    if (node->meshes.size() > 0)
    {
        DrawPropertyMesh(node->meshes);
    }
}
