#include "Common/Common.h"
#include "Common/Log.h"

#include "Core/Scene.h"

#include "Parser/stb_image_resize.h"

#include <iostream>
#include <algorithm>

GLScene::GLScene()
{
    
}

GLScene::~GLScene() 
{

}

bool GLScene::Init()
{
    m_SceneTextures = nullptr;

    // camera
    m_Camera = std::make_shared<Camera>();
    m_Camera->Perspective(MMath::DegreesToRadians(60.0f), 1.0f, 0.1f, 3000.0f);

    return true;
}

void GLScene::Free(bool freeHDR)
{
    m_SceneBvh        = nullptr;
    m_BvhTranslator   = nullptr;
    m_IndicesTexWidth = 0;
    m_TriDataTexWidth = 0;
    m_SceneBounds     = Bounds3D(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));

    m_Nodes.clear();
    m_Meshes.clear();
    m_Materials.clear();
    m_Lights.clear();
    m_Images.clear();
    m_Textures.clear();
    m_Renderers.clear();
    m_Indices.clear();
    m_Positions.clear();
    m_Normals.clear();
    m_Uvs.clear();
    m_Tangents.clear();
    m_Colors.clear();
    m_Transforms.clear();
    m_Scenes.clear();

    if (freeHDR)
    {
        m_Hdrs.clear();

        for (int32 i = 0; i < m_IBLs.size(); ++i)
        {
            m_IBLs[i]->Destroy();
            delete m_IBLs[i];
        }
        m_IBLs.clear();
    }

    if (m_SceneTextures)
    {
        delete m_SceneTextures;
        m_SceneTextures = nullptr;
    }

    for (size_t i = 0; i < m_VAOs.size(); ++i)
    {
        glDeleteVertexArrays(1, &(m_VAOs[i]));
    }
    m_VAOs.clear();

    for (size_t i = 0; i < m_VertexBuffers0.size(); ++i)
    {
        delete m_VertexBuffers0[i];
    }
    m_VertexBuffers0.clear();

    for (size_t i = 0; i < m_VertexBuffers1.size(); ++i)
    {
        delete m_VertexBuffers1[i];
    }
    m_VertexBuffers1.clear();

    for (size_t i = 0; i < m_VertexBuffers2.size(); ++i)
    {
        delete m_VertexBuffers2[i];
    }
    m_VertexBuffers2.clear();

    for (size_t i = 0; i < m_VertexBuffers3.size(); ++i)
    {
        delete m_VertexBuffers3[i];
    }
    m_VertexBuffers3.clear();

    for (size_t i = 0; i < m_VertexBuffers4.size(); ++i)
    {
        delete m_VertexBuffers4[i];
    }
    m_VertexBuffers4.clear();

    for (size_t i = 0; i < m_IndexBuffers.size(); ++i)
    {
        delete m_IndexBuffers[i];
    }
    m_IndexBuffers.clear();
}

void GLScene::AddScene(Scene3DPtr scene3D)
{
    m_Scenes.push_back(scene3D);

    for (size_t i = 0; i < scene3D->meshes.size(); ++i)
    {
        AddMesh(scene3D->meshes[i]);
    }

    for (size_t i = 0; i < scene3D->nodes.size(); ++i)
    {
        AddNode(scene3D->nodes[i]);
    }

    for (size_t i = 0; i < scene3D->images.size(); ++i)
    {
        AddImage(scene3D->images[i]);
    }

    for (size_t i = 0; i < scene3D->textures.size(); ++i)
    {
        AddTexture(scene3D->textures[i]);
    }
    
    for (size_t i = 0; i < scene3D->materials.size(); ++i)
    {
        AddMaterial(scene3D->materials[i]);
    }

    for (size_t i = 0; i < scene3D->lights.size(); ++i)
    {
        AddLight(scene3D->lights[i]);
    }

    for (size_t i = 0; i < scene3D->nodes.size(); ++i)
    {
        Object3DPtr node = scene3D->nodes[i];
        if (node->meshes.size() == 0)
        {
            continue;
        }

        for (size_t j = 0; j < node->meshes.size(); ++j)
        {
            auto mesh = node->meshes[j];
            auto mat  = node->materials[j];
            AddRenderer(mesh->id, mat->id, node->id);
        }
    }

    FitCamera();
}

void GLScene::FitCamera()
{
    Bounds3D bounds;
    for (size_t i = 0; i < m_Scenes.size(); ++i)
    {
        if (i == 0)
        {
            bounds = m_Scenes[i]->bounds;
        }
        else
        {
            bounds.Expand(m_Scenes[i]->bounds.min);
            bounds.Expand(m_Scenes[i]->bounds.max);
        }
    }

    // update position
    Vector3 center = bounds.Center();
    Vector3 extent = bounds.Extents();
    Vector3 eye    = Vector3(center.x, center.y, center.z - extent.Size() * 1.0f);
    Vector3 at     = center;

    // set up camera
    m_Camera->SetPosition(eye);
    m_Camera->LookAt(center);
}

int32 GLScene::AddMesh(MeshPtr mesh)
{
    int32 id = (int32)m_Meshes.size();
    mesh->id = id;
    m_Meshes.push_back(mesh);
    return id;
}

int32 GLScene::AddNode(Object3DPtr node)
{
    int32 id = (int32)m_Nodes.size();
    node->id = id;
    m_Nodes.push_back(node);
    return id;
}

int32 GLScene::AddImage(ImagePtr image)
{
    int32 id  = (int32)m_Images.size();
    image->id = id;
    m_Images.push_back(image);
    return id;
}

int32 GLScene::AddTexture(TexturePtr texture)
{
    int32 id = (int32)m_Textures.size();
    texture->id = id;
    m_Textures.push_back(texture);
    return id;
}

int32 GLScene::AddMaterial(MaterialPtr material)
{
    int32 id = (int32)m_Materials.size();
    material->id = id;
    m_Materials.push_back(material);
    return id;
}

int32 GLScene::AddRenderer(int32 mesh, int32 material, int32 node)
{
    RendererNode render;
    render.materialID = material;
    render.meshID     = mesh;
    render.nodeID     = node;

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

    IBLSampler* sampler = new IBLSampler();
    sampler->Init(hdr);
    m_IBLs.push_back(sampler);

    return id;
}

void GLScene::Build()
{
    CreateBLAS();
    BuildMesheDatas();
    BuildRendererDatas();
    GenVertexBuffers();
    GenIndexBuffers();
    GenTextureArrays();
}

void GLScene::BuildMesheDatas()
{
    // Copy mesh data
    int32 verticesCnt = 0;
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        // Copy m_Indices from BVH and not from Mesh
        const int32 numIndices  = (int32)m_Meshes[i]->bvh->GetNumIndices();
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
    m_IndicesTexWidth = (int32)(MMath::Sqrt((float)m_Indices.size()) + 1);
    m_TriDataTexWidth = (int32)(MMath::Sqrt((float)m_Positions.size()) + 1);

    m_Indices.resize(m_IndicesTexWidth * m_IndicesTexWidth);
    m_Positions.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Normals.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Uvs.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Tangents.resize(m_TriDataTexWidth * m_TriDataTexWidth);
    m_Colors.resize(m_TriDataTexWidth * m_TriDataTexWidth);

    // update indices
    for (size_t i = 0; i < m_Indices.size(); ++i)
    {
        m_Indices[i] = ((m_Indices[i] % m_TriDataTexWidth) << 12) | (m_Indices[i] / m_TriDataTexWidth);
    }
}

void GLScene::BuildRendererDatas()
{
    CreateTLAS();
    
    // Flatten BVH
    m_BvhTranslator = std::make_shared<BvhTranslator>();
    m_BvhTranslator->Process(m_SceneBvh, m_Meshes, m_Renderers);

    // Copy transforms
    m_Transforms.resize(m_Renderers.size());
    for (int32 i = 0; i < m_Renderers.size(); i++) 
    {
        m_Transforms[i] = m_Nodes[m_Renderers[i].nodeID]->GetGlobalTransform();
    }
}

void GLScene::RebuildRendererDatas()
{
    CreateTLAS();

    m_BvhTranslator->UpdateTLAS(m_SceneBvh, m_Renderers);

    for (int32 i = 0; i < m_Renderers.size(); i++) 
    {
        m_Transforms[i] = m_Nodes[m_Renderers[i].nodeID]->GetGlobalTransform();
    }
}

void GLScene::CreateTLAS()
{
    std::vector<Bounds3D> bounds;
    bounds.resize(m_Renderers.size());

    for (size_t i = 0; i < m_Renderers.size(); i++)
    {
        Bounds3D aabb    = m_Meshes[m_Renderers[i].meshID]->bvh->Bounds();
        Matrix4x4 matrix = m_Nodes[m_Renderers[i].nodeID]->GetGlobalTransform();
        Vector3 minBound = aabb.min;
        Vector3 maxBound = aabb.max;

        Vector3 right       = Vector3(matrix.m[0][0], matrix.m[0][1], matrix.m[0][2]);
        Vector3 up          = Vector3(matrix.m[1][0], matrix.m[1][1], matrix.m[1][2]);
        Vector3 forward     = Vector3(matrix.m[2][0], matrix.m[2][1], matrix.m[2][2]);
        Vector3 translation = Vector3(matrix.m[3][0], matrix.m[3][1], matrix.m[3][2]);

        Vector3 xa = minBound.x * right;
        Vector3 xb = maxBound.x * right;
        Vector3 ya = minBound.y * up;
        Vector3 yb = maxBound.y * up;
        Vector3 za = minBound.z * forward;
        Vector3 zb = maxBound.z * forward;

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

void GLScene::GenVertexBuffers()
{
    m_VAOs.resize(m_Meshes.size());
    m_VertexBuffers0.resize(m_Meshes.size());
    m_VertexBuffers1.resize(m_Meshes.size());
    m_VertexBuffers2.resize(m_Meshes.size());
    m_VertexBuffers3.resize(m_Meshes.size());
    m_VertexBuffers4.resize(m_Meshes.size());

    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        auto mesh = m_Meshes[i];

        // vbo
        {
            m_VertexBuffers0[i] = new VertexBuffer();
            m_VertexBuffers0[i]->Upload((uint8*)(mesh->positions.data()), (int32)(sizeof(Vector3) * mesh->positions.size()));

            m_VertexBuffers1[i] = new VertexBuffer();
            m_VertexBuffers1[i]->Upload((uint8*)(mesh->normals.data()), (int32)(sizeof(Vector3) * mesh->normals.size()));

            m_VertexBuffers2[i] = new VertexBuffer();
            m_VertexBuffers2[i]->Upload((uint8*)(mesh->uvs.data()), (int32)(sizeof(Vector2) * mesh->uvs.size()));

            m_VertexBuffers3[i] = new VertexBuffer();
            m_VertexBuffers3[i]->Upload((uint8*)(mesh->tangents.data()), (int32)(sizeof(Vector4) * mesh->tangents.size()));

            m_VertexBuffers4[i] = new VertexBuffer();
            m_VertexBuffers4[i]->Upload((uint8*)(mesh->colors.data()), (int32)(sizeof(Vector4) * mesh->colors.size()));
        }

        // vao
        {
            int32 offset = 0;

            glGenVertexArrays(1, &(m_VAOs[i]));
            glBindVertexArray(m_VAOs[i]);

            // positions
            glBindBuffer(m_VertexBuffers0[i]->Target(), m_VertexBuffers0[i]->Object());
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(0);

            // normals
            glBindBuffer(m_VertexBuffers1[i]->Target(), m_VertexBuffers1[i]->Object());
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(1);

            // uvs
            glBindBuffer(m_VertexBuffers2[i]->Target(), m_VertexBuffers2[i]->Object());
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(2);

            // tangents
            glBindBuffer(m_VertexBuffers3[i]->Target(), m_VertexBuffers3[i]->Object());
            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(3);

            // colors
            glBindBuffer(m_VertexBuffers4[i]->Target(), m_VertexBuffers4[i]->Object());
            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (GLvoid*)0);
            glEnableVertexAttribArray(4);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }
}

void GLScene::GenIndexBuffers()
{
    m_IndexBuffers.resize(m_Meshes.size());
    for (size_t i = 0; i < m_Meshes.size(); ++i)
    {
        m_IndexBuffers[i] = new IndexBuffer();
        m_IndexBuffers[i]->Upload((uint8*)(m_Meshes[i]->indices.data()), (int32)(m_Meshes[i]->indices.size() * sizeof(uint32)));
    }
}

void GLScene::GenTextureArrays()
{
    if (m_Images.size() == 0)
    {
        ImagePtr image = std::make_shared<Image>();
        image->comp   = 4;
        image->height = 8;
        image->name   = "default";
        image->width  = 8;
        for (int32 i = 0; i < 8; ++i)
        {
            for (int32 j = 0; j < 8; ++j) 
            {
                if ((j & 1) ^ (i & 1)) 
                {
                    image->rgba.push_back(0xFF);
                    image->rgba.push_back(0xFF);
                    image->rgba.push_back(0xFF);
                    image->rgba.push_back(0xFF);
                }
                else
                {
                    image->rgba.push_back(0);
                    image->rgba.push_back(0);
                    image->rgba.push_back(0);
                    image->rgba.push_back(0xFF);
                }
            }
        }
        AddImage(image);
    }

    int32 maxWidth  = 0;
    int32 maxHeight = 0;
    for (size_t i = 0; i < m_Images.size(); ++i)
    {
        auto image = m_Images[i];
        maxWidth   = MMath::Max(maxWidth,  image->width);
        maxHeight  = MMath::Max(maxHeight, image->height);
    }

    std::vector<uint8> tempData;
    tempData.resize(maxWidth * maxHeight * 4);
    for (size_t i = 0; i < m_Images.size(); ++i)
    {
        auto image = m_Images[i];
        if (image->width != maxWidth || image->height != maxHeight || image->comp != 4)
        {
            stbir_resize_uint8(image->rgba.data(), image->width, image->height, 0, tempData.data(), maxWidth, maxHeight, 0, 4);
            image->width  = maxWidth;
            image->height = maxHeight;
            image->comp   = 4;
            image->rgba.resize(tempData.size());
            memcpy(image->rgba.data(), tempData.data(), tempData.size());
        }
    }

    std::vector<uint8> arrayData;
    arrayData.reserve(m_Images.size() * tempData.size());
    for (size_t i = 0; i < m_Images.size(); ++i)
    {
        auto image = m_Images[i];
        arrayData.insert(arrayData.end(), image->rgba.begin(), image->rgba.end());
    }

    m_SceneTextures = new GLTexture(GL_TEXTURE_2D_ARRAY, GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, maxWidth, maxHeight, (int32)m_Images.size(), arrayData.data());
}
