#pragma once

#include "Base/Base.h"

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

    int32 AddImage(ImagePtr image);

    int32 AddMaterial(MaterialPtr material);

    int32 AddRenderer(int32 mesh, int32 material, const Matrix4x4& transform);

    int32 AddLight(LightPtr light);

    int32 AddHDR(HDRImagePtr hdr);

    void AddScene(Scene3DPtr scene3D);

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

private:

    void CreateAccelerationStructures();

    void RebuildInstancesData();

    void CreateBLAS();

    void CreateTLAS();

protected:

    MeshArray                       m_Meshes;
    MaterialArray                   m_Materials;
    LightArray                      m_Lights;
    ImageArray                      m_Images;
    HDRImageArray                   m_Hdrs;
    std::vector<RendererNode>       m_Renderers;
    CameraPtr					    m_Camera;

    std::vector<uint32>             m_Indices;
    std::vector<Vector3>            m_Positions;
    std::vector<uint32>             m_Normals;
    std::vector<Vector2>            m_Uvs;
    std::vector<uint32>             m_Tangents;
    std::vector<uint32>             m_Colors;
    std::vector<Matrix4x4>          m_Transforms;

    int32					        m_IndicesTexWidth;
    int32						    m_TriDataTexWidth;

    std::shared_ptr<BvhTranslator>  m_BvhTranslator;
    std::shared_ptr<Bvh>            m_SceneBvh;

    Bounds3D					    m_SceneBounds;
    Scene3DArray                    m_Scenes;
};

typedef std::shared_ptr<GLScene> GLScenePtr;
typedef std::vector<GLScenePtr>  GLSceneArray;
