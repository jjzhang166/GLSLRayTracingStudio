#pragma once

#include <vector>
#include <memory>

#include "Base/Buffer.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Bounds3D.h"
#include "Math/Matrix4x4.h"
#include "Bvh/SplitBvh.h"

struct Light;
struct Image;
struct HDRImage;
struct Texture;
struct Material;
struct Mesh;
struct Object3D;
struct Scene3D;

class  Camera;

typedef std::shared_ptr<Scene3D>    Scene3DPtr;
typedef std::vector<Scene3DPtr>     Scene3DArray;
typedef std::shared_ptr<Light>      LightPtr;
typedef std::vector<LightPtr>       LightArray;
typedef std::shared_ptr<Image>      ImagePtr;
typedef std::vector<ImagePtr>       ImageArray;
typedef std::shared_ptr<HDRImage>   HDRImagePtr;
typedef std::vector<HDRImagePtr>    HDRImageArray;
typedef std::shared_ptr<Texture>    TexturePtr;
typedef std::vector<TexturePtr>     TextureArray;
typedef std::shared_ptr<Material>   MaterialPtr;
typedef std::vector<MaterialPtr>    MaterialArray;
typedef std::shared_ptr<Mesh>       MeshPtr;
typedef std::vector<MeshPtr>        MeshArray;
typedef std::shared_ptr<Object3D>   Object3DPtr;
typedef std::vector<Object3DPtr>    Object3DArray;
typedef std::shared_ptr<Camera>     CameraPtr;
typedef std::vector<CameraPtr>      CameraArray;

enum class DebugMode
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

struct RendererNode
{
    int32                   nodeID = -1;
    int32                   meshID = -1;
    int32                   materialID = -1;
};

struct Light
{
    enum LightType
    {
        DIRECTIONAL = 0,
        POINT,
        SPOT
    };

    Object3DPtr             node = nullptr;

    int32                   type = LightType::DIRECTIONAL;

    Vector3                 color = Vector3(1.0f, 1.0f, 1.0f);
    float                   intensity = 1.0f;

    float                   range = 0.0f;
    float                   innerCone = 0.0f;
    float                   outerCone = 0.0f;
};

struct Image
{
    int32                   id;

    std::string             name;
    int32                   width = 0;
    int32                   height = 0;
    int32                   comp = 4;
    std::vector<uint8>      rgba;
};

struct HDRImage
{
    int32                   id;

    std::string             name;
    int32                   width;
    int32                   height;
    int32                   component;
    std::vector<float>      hdrRGB;
    std::vector<float>      envRGB;
};

struct Texture
{
    int32                   id;

    ImagePtr                source = nullptr;
    int32                   minFilter = -1;
    int32                   magFilter = -1;
    int32                   wrapS = -1;
    int32                   wrapT = -1;
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

    int32                   id;

    // 0
    Vector4                 pbrBaseColorFactor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    // 4
    int32                   pbrBaseColorTexture = -1;
    float                   pbrMetallicFactor = 1.0f;
    float                   pbrRoughnessFactor = 1.0f;
    int32                   pbrMetallicRoughnessTexture = -1;
    // 8
    Vector4                 pbrDiffuseFactor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    Vector3                 pbrSpecularFactor = Vector3(1.0f, 1.0f, 1.0f);
    int32                   pbrDiffuseTexture = -1;
    // 16
    int32                   shadingModel = MaterialType::METALLICROUGHNESS;
    float                   pbrGlossinessFactor = 1.0f;
    int32                   pbrSpecularGlossinessTexture = -1;
    int32                   emissiveTexture = -1;
    // 20
    Vector3                 emissiveFactor = Vector3(1.0f, 1.0f, 1.0f);
    int32                   alphaMode = AlphaType::NONE;
    // 24
    float                   alphaCutoff = 0.5f;
    int32                   doubleSided = 0;
    int32                   normalTexture = -1;
    float                   normalTextureScale = 1.0f;
    // 28
    Vector4                 offsetScale = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    // 32
    int32                   unlit = -1;
    float                   transmissionFactor = 1.0f;
    int32                   transmissionTexture = -1;
    float                   ior = 1.0f;
    // 36
    Vector3                 anisotropyDirection = Vector3(0.0f, 0.0f, 0.0f);
    float                   anisotropy = 0.0f;
    // 40
    Vector3                 attenuationColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    float                   thicknessFactor = 1.0f;
    // 44
    int32                   thicknessTexture = -1;
    float                   attenuationDistance = 0;
    float                   clearcoatFactor = 1.0f;
    float                   clearcoatRoughness = 1.0f;
    // 48
    int32                   clearcoatTexture = -1;
    int32                   clearcoatRoughnessTexture = -1;
};

struct Mesh
{
    int32                   id;

    std::string             name;
    Object3DPtr             node = nullptr;
    std::shared_ptr<Bvh>    bvh = nullptr;
    int32                   material = -1;
    Bounds3D                aabb = Bounds3D(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 0.0f));
    std::vector<uint32>     indices;
    std::vector<Vector3>    positions;
    std::vector<Vector3>    normals;
    std::vector<Vector2>    uvs;
    std::vector<Vector4>    tangents;
    std::vector<Vector4>    colors;

    void BuildBVH()
    {
        const int32 numTris = (int32)indices.size() / 3;
        std::vector<Bounds3D> bounds(numTris);

        for (int32 i = 0; i < numTris; ++i)
        {
            uint32 idx0 = indices[i * 3 + 0];
            uint32 idx1 = indices[i * 3 + 1];
            uint32 idx2 = indices[i * 3 + 2];

            const auto& p0 = positions[idx0];
            const auto& p1 = positions[idx0];
            const auto& p2 = positions[idx0];

            bounds[i].Expand(p0);
            bounds[i].Expand(p1);
            bounds[i].Expand(p2);
        }

        bvh = std::make_shared<SplitBvh>(2.0f, 64, 0, 0.001f, 2.5f);
        bvh->Build(&bounds[0], numTris);
    }
};

class Camera
{
public:
    Camera();

    void TranslateX(float distance);

    void TranslateY(float distance);

    void TranslateZ(float distance);

    void RotateX(float angle);

    void RotateY(float angle);

    void RotateZ(float angle);

    void LookAt(float x, float y, float z, float smooth = 1.0f);

    void LookAt(const Vector3& target, float smooth = 1.0f);

    void LookAt(const Vector3& target, const Vector3& up, float smooth = 1.0f);

    Vector3 GetPosition();

    void SetPosition(const Vector3& pos);

    void SetPosition(float x, float y, float z);

    void SetOrientation(const Vector3& dir);

    void SetRotation(const Vector3& rotation);

    void SetRotation(float eulerX, float eulerY, float eulerZ);

    Vector3 GetRight() const;

    Vector3 GetUp() const;

    Vector3 GetForward() const;

    Vector3 GetLeft() const;

    Vector3 GetBackward() const;

    Vector3 GetDown() const;

    const Matrix4x4& GetView();

    const Matrix4x4& GetProjection();

    const Matrix4x4& GetViewProjection();

    void SetTransform(const Matrix4x4& world);

    Matrix4x4 GetTransform();

    void Perspective(float fovy, float aspect, float zNear, float zFar);

    void SetFov(float fov);

    void SetAspect(float aspect);

    void GetGizmoViewProjection(float* view, float* projection);

    void Update(float delta);

    void OnMousePos(const Vector2 mousePos, bool forceLast);

    void OnRMouse(bool down);

    void OnMMouse(bool down);

    void OnMouseWheel(float wheel);

    FORCEINLINE float GetNear() const
    {
        return m_Near;
    }

    FORCEINLINE float GetFar() const
    {
        return m_Far;
    }

    FORCEINLINE float GetFov() const
    {
        return m_Fov;
    }

    FORCEINLINE float GetAspect() const
    {
        return m_Aspect;
    }

public:

    float		            smooth = 1.0f;
    float		            speed = 1.0f;
    float		            speedFactor = 0.5f;

    float		            focalDist = 1.0f;
    float		            aperture = 0.0f;

    bool                    isMoving = false;

protected:

    bool                    m_RMouseDown = false;
    bool                    m_MMouseDown = false;

    Vector2		            m_LastMouse;
    Vector2                 m_CurrMouse;
    float		            m_MouseWheel = 0.0f;

    float		            m_SpinX = 0.0f;
    float		            m_SpinY = 0.0f;
    float		            m_SpinZ = 0.0f;

    Matrix4x4               m_Transform;
    Matrix4x4	            m_View;
    Matrix4x4	            m_Projection;
    Matrix4x4	            m_ViewProjection;

    float		            m_Near = 1.0f;
    float		            m_Far = 3000.0f;

    // Perspective
    float		            m_Fov = PI / 4.0f;
    float		            m_Aspect = 1.0f;
};

struct Object3D
{
    Object3D();

    int32                   id = -1;

    int32                   instanceID = -1;
    std::string             name;
    Matrix4x4               transform;
    LightPtr                light = nullptr;
    CameraPtr               camera = nullptr;
    Object3DPtr             parent = nullptr;
    Object3DArray           children;
    MaterialArray           materials;
    MeshArray               meshes;

    Matrix4x4               globalTransform;
    bool                    globalTransformDirty = true;
   
    const Matrix4x4& GetGlobalTransform()
    {
        if (globalTransformDirty)
        {
            globalTransformDirty = false;
            globalTransform = transform;
            if (parent)
            {
                globalTransform.Append(parent->GetGlobalTransform());
            }
        }

        return globalTransform;
    }

    const Matrix4x4& GetLocalTransform()
    {
        return transform;
    }

    void SetPosition(const Vector3& pos)
    {
        transform.SetPosition(pos);
        InvalidTransform();
    }

    void SetRotation(const Vector3& rot)
    {
        transform.SetRotation(rot);
        InvalidTransform();
    }

    void SetScale(const Vector3& sca)
    {
        transform.SetScale(sca);
        InvalidTransform();
    }

    void InvalidTransform()
    {
        globalTransformDirty = true;
        for (size_t i = 0; i < children.size(); ++i)
        {
            children[i]->InvalidTransform();
        }
    }
};

struct Scene3D
{
    Bounds3D                bounds;
    Object3DPtr             rootNode = nullptr;
    Object3DArray           nodes;
    MeshArray               meshes;
    MaterialArray           materials;
    ImageArray              images;
    TextureArray            textures;
    LightArray              lights;
    CameraArray             cameras;
};

