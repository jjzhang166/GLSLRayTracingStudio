#include "Base/Base.h"

#include "Bvh/Bvh.h"
#include "Bvh/BvhTranslator.h"

#include "Core/Texture.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <string>

class Scene
{
public:
    Scene();

    virtual ~Scene();

    void SetCamera(CameraPtr camera);

    int32 AddMesh(MeshPtr mesh);

    int32 AddImage(ImagePtr image);

    int32 AddMaterial(MaterialPtr material);

    int32 AddRenderer(int32 mesh, int32 material, const Matrix4x4& transform);

    int32 AddLight(LightPtr light);

    int32 AddHDR(HDRImagePtr hdr);

    void CreateAccelerationStructures();

    void RebuildInstancesData();

private:

    void CreateBLAS();

    void CreateTLAS();

public:

    MeshArray                       meshes;
    MaterialArray                   materials;
    LightArray                      lights;
    ImageArray                      images;
    HDRImageArray                   hdrs;
    std::vector<RendererNode>       renderers;
    CameraPtr					    camera;

    std::vector<uint32>             indices;
    std::vector<Vector3>            positions;
    std::vector<uint32>             normals;
    std::vector<Vector2>            uvs;
    std::vector<uint32>             tangents;
    std::vector<uint32>             colors;
    std::vector<Matrix4x4>          transforms;

    int32					        indicesTexWidth;
    int32						    triDataTexWidth;

    std::shared_ptr<BvhTranslator>  bvhTranslator;
    std::shared_ptr<Bvh>            sceneBvh;

    Bounds3D					    sceneBounds;
};
