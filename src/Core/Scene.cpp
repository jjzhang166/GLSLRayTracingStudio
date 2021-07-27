#include "Common/Common.h"
#include "Common/Log.h"
#include "Core/Scene.h"

#include <iostream>
#include <algorithm>

GLScene::GLScene() 
{
    
}

GLScene::~GLScene() 
{

}

void GLScene::AddScene(Scene3DPtr scene3D)
{

}

void GLScene::SetCamera(CameraPtr inCamera)
{
    m_Camera = inCamera;
}

int32 GLScene::AddMesh(MeshPtr mesh)
{
    int32 id = (int32)m_Meshes.size();
    m_Meshes.push_back(mesh);
    return id;
}

int32 GLScene::AddImage(ImagePtr image)
{
    int32 id = (int32)m_Images.size();
    m_Images.push_back(image);
    return id;
}

int32 GLScene::AddMaterial(MaterialPtr material)
{
    int32 id = (int32)m_Materials.size();
    m_Materials.push_back(material);
    return id;
}

int32 GLScene::AddRenderer(int32 mesh, int32 material, const Matrix4x4& transform)
{
    RendererNode render;
    render.materialID = material;
    render.meshID     = mesh;
    render.transform  = transform;

    int32 id = (int32)m_Renderers.size();
    m_Renderers.push_back(render);
    return id;
}

int32 GLScene::AddLight(LightPtr light)
{
    int32 id = (int32)m_Lights.size();
    m_Lights.push_back(light);
    return id;
}

int32 GLScene::AddHDR(HDRImagePtr hdr)
{
    int32 id = (int32)m_Hdrs.size();
    m_Hdrs.push_back(hdr);
    return id;
}

void GLScene::CreateAccelerationStructures()
{
    CreateBLAS();
    CreateTLAS();

    // Flatten BVH
    m_BvhTranslator = std::make_shared<BvhTranslator>();
    m_BvhTranslator->Process(m_SceneBvh, m_Meshes, m_Renderers);

    // Copy mesh data
    int32 verticesCnt = 0;
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        // Copy m_Indices from BVH and not from Mesh
        int32 numIndices = (int32)m_Meshes[i]->bvh->GetNumIndices();
        const int32* triIndices = m_Meshes[i]->bvh->GetIndices();

        for (int32 j = 0; j < numIndices; ++j)
        {
            m_Indices.push_back(triIndices[j] + verticesCnt);
        }

        verticesCnt += (int32)m_Meshes[i]->positions.size();

        m_Positions.insert(m_Positions.begin(), m_Meshes[i]->positions.begin(), m_Meshes[i]->positions.end());
        m_Normals.insert(m_Normals.begin(), m_Meshes[i]->normals.begin(), m_Meshes[i]->normals.end());
        m_Uvs.insert(m_Uvs.begin(), m_Meshes[i]->uvs.begin(), m_Meshes[i]->uvs.end());
        m_Tangents.insert(m_Tangents.begin(), m_Meshes[i]->tangents.begin(), m_Meshes[i]->tangents.end());
        m_Colors.insert(m_Colors.begin(), m_Meshes[i]->colors.begin(), m_Meshes[i]->colors.end());
    }

    // Resize to power of 2
    // TODO:pack data
    m_IndicesTexWidth = (int32)(MMath::Sqrt((float)m_Indices.size()) + 1);
    m_TriDataTexWidth = (int32)(MMath::Sqrt((float)m_Positions.size()) + 1);

    m_Indices.resize(m_IndicesTexWidth * m_IndicesTexWidth);
    m_Positions.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Normals.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Uvs.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Tangents.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Colors.resize(m_TriDataTexWidth * m_TriDataTexWidth);

    for (size_t i = 0; i < m_Indices.size(); ++i)
    {
        m_Indices[i] = ((m_Indices[i] % m_TriDataTexWidth) << 12) | (m_Indices[i] / m_TriDataTexWidth);
    }

    // Copy transforms
    m_Transforms.resize(m_Renderers.size());
    for (int32 i = 0; i < m_Renderers.size(); i++) 
    {
        m_Transforms[i] = m_Renderers[i].transform;
    }
}

void GLScene::RebuildInstancesData()
{
    CreateTLAS();

    m_BvhTranslator->UpdateTLAS(m_SceneBvh, m_Renderers);

    for (int32 i = 0; i < m_Renderers.size(); i++) 
    {
        m_Transforms[i] = m_Renderers[i].transform;
    }
}

void GLScene::CreateTLAS()
{
    std::vector<Bounds3D> bounds;
    bounds.resize(m_Renderers.size());

    for (size_t i = 0; i < m_Renderers.size(); i++)
    {
        Bounds3D aabb    = m_Meshes[m_Renderers[i].meshID]->bvh->Bounds();
        Matrix4x4 matrix = m_Renderers[i].transform;
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

    m_SceneBvh = std::make_shared<Bvh>(10.0f, 64, false);
    m_SceneBvh->Build(&bounds[0], (int32)bounds.size());

    m_SceneBounds = m_SceneBvh->Bounds();
}

void GLScene::CreateBLAS()
{
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        auto mesh = m_Meshes[i];
        if (mesh->bvh)
        {
            continue;
        }
        mesh->BuildBVH();
    }
}
