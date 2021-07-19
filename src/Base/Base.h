#pragma once

#include <vector>
#include <memory>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Bvh/SplitBvh.h"

struct Light;
struct Texture;
struct Material;
struct Mesh;
struct Object3D;
struct Scene3D;

typedef std::shared_ptr<Scene3D>    Scene3DPtr;
typedef std::shared_ptr<Light>      LightPtr;
typedef std::vector<LightPtr>       LightArray;
typedef std::shared_ptr<Texture>    TexturePtr;
typedef std::vector<TexturePtr>     TextureArray;
typedef std::shared_ptr<Material>   MaterialPtr;
typedef std::vector<MaterialPtr>    MaterialArray;
typedef std::shared_ptr<Mesh>       MeshPtr;
typedef std::vector<MeshPtr>        MeshArray;
typedef std::shared_ptr<Object3D>   Object3DPtr;
typedef std::vector<Object3DPtr>    Object3DArray;

enum DebugMode
{
    ENoDebug   = 0,
    EBaseColor = 1,
    ENormal    = 2,
    EMetallic  = 3,
    EEmissive  = 4,
    EAlpha     = 5,
    ERoughness = 6,
    ETexcoord  = 7,
    ETangent   = 8,
    ERadiance  = 9,
    EWeight    = 10,
    ERayDir    = 11
};

struct Light
{
    enum LightType
    {
        DIRECTIONAL = 0,
        POINT,
        SPOT
    };

    Vector3         direction;
    float           range;

    Vector3         color;
    float           intensity;

    Vector3         position;
    float           innerConeCos;

    float           outerConeCos;
    int32           type;

    Vector2         padding;
};

struct Texture
{
    std::string         name;
    int32               width;
	int32               height;
	int32               comp;
	std::vector<uint8>  rgba;
};

struct Material
{
    enum MaterialType
    {
        METALLICROUGHNESS = 0,
        SPECULARGLOSSINESS
    };

    enum AlphaType
    {
        NONE = 0,
        MASK,
        BLEND
    };

    // 0
    Vector4         pbrBaseColorFactor;
    // 4
    int32           pbrBaseColorTexture;
    float           pbrMetallicFactor;
    float           pbrRoughnessFactor;
    int32           pbrMetallicRoughnessTexture;
    // 8
    Vector4         pbrDiffuseFactor;
    Vector3         pbrSpecularFactor;
    int32           pbrDiffuseTexture;
    // 16
    int32           shadingModel;
    float           pbrGlossinessFactor;
    int32           pbrSpecularGlossinessTexture;
    int32           emissiveTexture;
    // 20
    Vector3         emissiveFactor;
    int32           alphaMode;
    // 24
    float           alphaCutoff;
    int32           doubleSided;
    int32           normalTexture;
    float           normalTextureScale;
    // 28
    Matrix4x4       uvTransform;
    // 32
    int32           unlit;
    float           transmissionFactor;
    int32           transmissionTexture;
    float           ior;
    // 36
    Vector3         anisotropyDirection;
    float           anisotropy;
    // 40
    Vector3         attenuationColor;
    float           thicknessFactor;
    // 44
    int32           thicknessTexture;
    float           attenuationDistance;
    float           clearcoatFactor;
    float           clearcoatRoughness;
    // 48
    int32           clearcoatTexture;
    int32           clearcoatRoughnessTexture;
    int32           pad0;
    int32           pad1;
    // 52
};

struct Mesh
{
    std::string             name;
    Bvh*                    bvh;
    std::vector<Vector3>    vertices;
    std::vector<uint32>     normals;
    std::vector<Vector2>    uvs;
    std::vector<uint32>     tangent;
    std::vector<uint32>     color;
};

struct MeshInstance
{
    std::string             name;
    int32                   meshID;
    int32                   materialID;
    Matrix4x4	            transform;
};

struct Object3D
{
    std::string             name;
    Matrix4x4               transform;
    Bounds3D                bounds;
    MaterialArray           materials;
    MeshArray               meshes;
    Object3DPtr             parent;
    Object3DArray           children;

    Matrix4x4 GlobalTransform()
    {
        Matrix4x4 globalTransform = transform;
        if (parent)
        {
            globalTransform.Append(parent->GlobalTransform());
        }
        return globalTransform;
    }

    Matrix4x4 LocalTransform()
    {
        return transform;
    }
};

struct Scene3D
{
    Object3DPtr             rootNode;
    Object3DArray           nodes;
    MeshArray               meshes;
    MaterialArray           materials;
    TextureArray            textures;

    int32                   numCameras;
    int32                   numImages;
    int32                   numTextures;
    int32                   numMaterials;
    int32                   numSamplers;
    int32                   numNodes;
    int32                   numMeshes;
    int32                   numLights;
};
