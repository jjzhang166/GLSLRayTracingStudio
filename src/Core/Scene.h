#pragma once

#include "Base/Base.h"
#include "Base/Buffer.h"

#include "Bvh/Bvh.h"
#include "Bvh/BvhTranslator.h"

#include "Core/Texture.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <string>

class GLScene
{
public:
    GLScene();

    virtual ~GLScene();

    bool Init();

    void Free();

    void SetCamera(CameraPtr camera);

    int32 AddMesh(MeshPtr mesh);

    int32 AddNode(Object3DPtr node);

    int32 AddImage(ImagePtr image);

    int32 AddTexture(TexturePtr texture);

    int32 AddMaterial(MaterialPtr material);

    int32 AddRenderer(int32 mesh, int32 material, int32 node);

    int32 AddLight(LightPtr light);

    int32 AddHDR(HDRImagePtr hdr);

    void AddScene(Scene3DPtr scene3D);

    void Build();

    void RebuildRendererDatas();

    FORCEINLINE CameraPtr GetCamera() const
    {
        return m_Camera;
    }

    FORCEINLINE const Scene3DArray& GetScenes() const
    {
        return m_Scenes;
    }

    FORCEINLINE Scene3DArray& GetScenes()
    {
        return m_Scenes;
    }

    FORCEINLINE const Object3DArray& Nodes() const
    {
        return m_Nodes;
    }

    FORCEINLINE const MeshArray& Meshes() const
    {
        return m_Meshes;
    }

    FORCEINLINE const std::vector<RendererNode>& Renderers() const
    {
        return m_Renderers;
    }

    FORCEINLINE const GLTexture* SceneTextures() const
    {
        return m_SceneTextures;
    }

    FORCEINLINE const std::vector<GLuint>& VAOs() const
    {
        return m_VAOs;
    }

    FORCEINLINE const std::vector<VertexBuffer*>& VertexBuffers0() const
    {
        return m_VertexBuffers0;
    }

    FORCEINLINE const std::vector<VertexBuffer*>& VertexBuffers1() const
    {
        return m_VertexBuffers1;
    }

    FORCEINLINE const std::vector<VertexBuffer*>& VertexBuffers2() const
    {
        return m_VertexBuffers2;
    }

    FORCEINLINE const std::vector<VertexBuffer*>& VertexBuffers3() const
    {
        return m_VertexBuffers3;
    }

    FORCEINLINE const std::vector<VertexBuffer*>& VertexBuffers4() const
    {
        return m_VertexBuffers4;
    }

    FORCEINLINE const std::vector<IndexBuffer*>& IndexBuffers() const
    {
        return m_IndexBuffers;
    }

private:

    void CreateBLAS();

    void CreateTLAS();

    void BuildMesheDatas();

    void BuildRendererDatas();

    void GenVertexBuffers();

    void GenIndexBuffers();

    void GenTextureArrays();
    
protected:

    MeshArray                       m_Meshes;
    Object3DArray                   m_Nodes;
    MaterialArray                   m_Materials;
    LightArray                      m_Lights;
    ImageArray                      m_Images;
    TextureArray                    m_Textures;
    HDRImageArray                   m_Hdrs;
    CameraPtr					    m_Camera;
    std::vector<RendererNode>       m_Renderers;

    std::vector<uint32>             m_Indices;
    std::vector<Vector3>            m_Positions;
    std::vector<Vector3>            m_Normals;
    std::vector<Vector2>            m_Uvs;
    std::vector<Vector4>            m_Tangents;
    std::vector<Vector4>            m_Colors;
    std::vector<Matrix4x4>          m_Transforms;

    int32					        m_IndicesTexWidth;
    int32						    m_TriDataTexWidth;

    std::shared_ptr<BvhTranslator>  m_BvhTranslator;
    std::shared_ptr<Bvh>            m_SceneBvh;

    std::vector<VertexBuffer*>      m_VertexBuffers0;
    std::vector<VertexBuffer*>      m_VertexBuffers1;
    std::vector<VertexBuffer*>      m_VertexBuffers2;
    std::vector<VertexBuffer*>      m_VertexBuffers3;
    std::vector<VertexBuffer*>      m_VertexBuffers4;
    std::vector<IndexBuffer*>       m_IndexBuffers;
    std::vector<GLuint>             m_VAOs;
    GLTexture*                      m_SceneTextures;

    Bounds3D					    m_SceneBounds;
    Scene3DArray                    m_Scenes;
};

typedef std::shared_ptr<GLScene> GLScenePtr;
typedef std::vector<GLScenePtr>  GLSceneArray;
