#include "Common/Common.h"
#include "Common/Log.h"
#include "Core/Scene.h"

#include <iostream>
#include <algorithm>

Scene::Scene() 
{
    
}

Scene::~Scene() 
{

}

void Scene::SetCamera(CameraPtr inCamera)
{
    camera = inCamera;
}

int32 Scene::AddMesh(MeshPtr mesh)
{
    int32 id = (int32)meshes.size();
    meshes.push_back(mesh);
    return id;
}

int32 Scene::AddImage(ImagePtr image)
{
    int32 id = (int32)images.size();
    images.push_back(image);
    return id;
}

int32 Scene::AddMaterial(MaterialPtr material)
{
    int32 id = (int32)materials.size();
    materials.push_back(material);
    return id;
}

int32 Scene::AddRenderer(int32 mesh, int32 material, const Matrix4x4& transform)
{
    RendererNode render;
    render.materialID = material;
    render.meshID     = mesh;
    render.transform  = transform;

    int32 id = (int32)renderers.size();
    renderers.push_back(render);
    return id;
}

int32 Scene::AddLight(LightPtr light)
{
    int32 id = (int32)lights.size();
    lights.push_back(light);
    return id;
}

int32 Scene::AddHDR(HDRImagePtr hdr)
{
    int32 id = (int32)hdrs.size();
    hdrs.push_back(hdr);
    return id;
}

void Scene::CreateAccelerationStructures()
{
    CreateBLAS();
    CreateTLAS();

    // Flatten BVH
    bvhTranslator = std::make_shared<BvhTranslator>();
    bvhTranslator->Process(sceneBvh, meshes, renderers);

    // Copy mesh data
    int32 verticesCnt = 0;
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        // Copy indices from BVH and not from Mesh
        int32 numIndices = (int32)meshes[i]->bvh->GetNumIndices();
        const int32* triIndices = meshes[i]->bvh->GetIndices();

        for (int32 j = 0; j < numIndices; ++j)
        {
            indices.push_back(triIndices[j] + verticesCnt);
        }

        verticesCnt += (int32)meshes[i]->positions.size();

        positions.insert(positions.begin(), meshes[i]->positions.begin(), meshes[i]->positions.end());
        normals.insert(normals.begin(), meshes[i]->normals.begin(), meshes[i]->normals.end());
        uvs.insert(uvs.begin(), meshes[i]->uvs.begin(), meshes[i]->uvs.end());
        tangents.insert(tangents.begin(), meshes[i]->tangents.begin(), meshes[i]->tangents.end());
        colors.insert(colors.begin(), meshes[i]->colors.begin(), meshes[i]->colors.end());
    }

    // Resize to power of 2
    // TODO:pack data
    indicesTexWidth = (int32)(MMath::Sqrt(indices.size()) + 1);
    triDataTexWidth = (int32)(MMath::Sqrt(positions.size()) + 1);

    indices.resize(indicesTexWidth * indicesTexWidth);
    positions.resize(triDataTexWidth * triDataTexWidth);
    normals.resize(triDataTexWidth * triDataTexWidth);
    uvs.resize(triDataTexWidth * triDataTexWidth);
    tangents.resize(triDataTexWidth * triDataTexWidth);
    colors.resize(triDataTexWidth * triDataTexWidth);

    for (size_t i = 0; i < indices.size(); ++i)
    {
        indices[i] = ((indices[i] % triDataTexWidth) << 12) | (indices[i] / triDataTexWidth);
    }

    // Copy transforms
    transforms.resize(renderers.size());
    for (int32 i = 0; i < renderers.size(); i++) 
    {
        transforms[i] = renderers[i].transform;
    }
}

void Scene::RebuildInstancesData()
{
    CreateTLAS();

    bvhTranslator->UpdateTLAS(sceneBvh, renderers);

    for (int32 i = 0; i < renderers.size(); i++) 
    {
        transforms[i] = renderers[i].transform;
    }
}

void Scene::CreateTLAS()
{
    std::vector<Bounds3D> bounds;
    bounds.resize(renderers.size());

    for (size_t i = 0; i < renderers.size(); i++)
    {
        Bounds3D aabb    = meshes[renderers[i].meshID]->bvh->Bounds();
        Matrix4x4 matrix = renderers[i].transform;
        Vector3 minBound = aabb.min;
        Vector3 maxBound = aabb.max;

        Vector3 right       = Vector3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
        Vector3 up          = Vector3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
        Vector3 forward     = Vector3(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);
        Vector3 translation = Vector3(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);

        Vector3 xa = right * minBound.x;
        Vector3 xb = right * maxBound.x;
        Vector3 ya = up * minBound.y;
        Vector3 yb = up * maxBound.y;
        Vector3 za = forward * minBound.z;
        Vector3 zb = forward * maxBound.z;

        bounds[i].min = Vector3::Min(xa, xb) + Vector3::Min(ya, yb) + Vector3::Min(za, zb) + translation;
        bounds[i].max = Vector3::Max(xa, xb) + Vector3::Max(ya, yb) + Vector3::Max(za, zb) + translation;
    }

    sceneBvh = std::make_shared<Bvh>(10.0f, 64, false);
    sceneBvh->Build(&bounds[0], (int32)bounds.size());

    sceneBounds = sceneBvh->Bounds();
}

void Scene::CreateBLAS()
{
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        auto mesh = meshes[i];
        if (mesh->bvh)
        {
            continue;
        }
        mesh->BuildBVH();
    }
}
