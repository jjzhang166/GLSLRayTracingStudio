#include "Parser/GLTFParser.h"
#include "Parser/tiny_gltf.h"

#include "Misc/FileMisc.h"
#include "Base/Base.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"

#include <string>

#define KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME "KHR_lights_punctual"
#define KHR_MATERIALS_PBRSPECULARGLOSSINESS_EXTENSION_NAME "KHR_materials_pbrSpecularGlossiness"
#define KHR_TEXTURE_TRANSFORM_EXTENSION_NAME "KHR_texture_transform"
#define KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME "KHR_materials_clearcoat"
#define KHR_MATERIALS_SHEEN_EXTENSION_NAME "KHR_materials_sheen"
#define KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME "KHR_materials_transmission"
#define KHR_MATERIALS_UNLIT_EXTENSION_NAME "KHR_materials_unlit"
#define KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME "KHR_materials_anisotropy"
#define KHR_MATERIALS_IOR_EXTENSION_NAME "KHR_materials_ior"
#define KHR_MATERIALS_VOLUME_EXTENSION_NAME "KHR_materials_volume"

template <typename T>
static FORCEINLINE std::vector<T> GetGLTFVector(const tinygltf::Value& value)
{
    std::vector<T> result{0};
    if (!value.IsArray())
    {
        return result;
    }

    result.resize(value.ArrayLen());
    for (int i = 0; i < value.ArrayLen(); i++)
    {
        result[i] = static_cast<T>(value.Get(i).IsNumber() ? value.Get(i).Get<double>() : value.Get(i).Get<int>());
    }
    return result;
}

static FORCEINLINE void GetGLTFFloat(const tinygltf::Value& value, const std::string& name, float& val)
{
    if (value.Has(name))
    {
        val = static_cast<float>(value.Get(name).Get<double>());
    }
}

static FORCEINLINE void GetGLTFInt(const tinygltf::Value& value, const std::string& name, int& val)
{
    if (value.Has(name))
    {
        val = value.Get(name).Get<int>();
    }
}

static FORCEINLINE void GetGLTFVec2(const tinygltf::Value& value, const std::string& name, Vector2& val)
{
    if (value.Has(name))
    {
        auto s = GetGLTFVector<float>(value.Get(name));
        val    = Vector2(s[0], s[1]);
    }
}

static FORCEINLINE void GetGLTFVec3(const tinygltf::Value& value, const std::string& name, Vector3& val)
{
    if (value.Has(name))
    {
        auto s = GetGLTFVector<float>(value.Get(name));
        val    = Vector3(s[0], s[1], s[2]);
    }
}

static FORCEINLINE void GetGLTFVec4(const tinygltf::Value& value, const std::string& name, Vector4& val)
{
    if (value.Has(name))
    {
        auto s = GetGLTFVector<float>(value.Get(name));
        val    = Vector4(s[0], s[1], s[2], s[3]);
    }
}

static FORCEINLINE void GetGLTFTexId(const tinygltf::Value& value, const std::string& name, int& val)
{
    if (value.Has(name))
    {
        val = value.Get(name).Get("index").Get<int>();
    }
}

static void ImportMaterials(Scene3DPtr scene, tinygltf::Model& model)
{
    scene->materials.reserve(int32(model.materials.size()));

    for (int32 i = 0; i < (int32)model.materials.size(); ++i)
    {
        auto material = std::make_shared<Material>();
        auto& gltfMat = model.materials[i];

        scene->materials.push_back(material);

        material->alphaCutoff        = float(gltfMat.alphaCutoff);
        material->alphaMode          = gltfMat.alphaMode == "MASK" ? (int32)Material::AlphaType::MASK : (gltfMat.alphaMode == "BLEND" ? (int32)Material::AlphaType::BLEND : (int32)Material::AlphaType::NONE);
        material->doubleSided        = gltfMat.doubleSided ? 1 : 0;
        material->emissiveFactor     = Vector3(gltfMat.emissiveFactor[0], gltfMat.emissiveFactor[1], gltfMat.emissiveFactor[2]);
        material->emissiveTexture    = gltfMat.emissiveTexture.index;
        material->normalTexture      = gltfMat.normalTexture.index;
        material->normalTextureScale = float(gltfMat.normalTexture.scale);
        // Raytracing don't need this.
        // gltfMat.occlusionTexture.index;
        // gltfMat.occlusionTexture.strength;

        // PbrMetallicRoughness
        auto& tpbr = gltfMat.pbrMetallicRoughness;
        material->pbrBaseColorFactor            = Vector4(tpbr.baseColorFactor[0], tpbr.baseColorFactor[1], tpbr.baseColorFactor[2], tpbr.baseColorFactor[3]);
        material->pbrBaseColorTexture           = tpbr.baseColorTexture.index;
        material->pbrMetallicFactor             = float(tpbr.metallicFactor);
        material->pbrMetallicRoughnessTexture   = tpbr.metallicRoughnessTexture.index;
        material->pbrRoughnessFactor            = float(tpbr.roughnessFactor);

        // KHR_materials_pbrSpecularGlossiness
        if (gltfMat.extensions.find(KHR_MATERIALS_PBRSPECULARGLOSSINESS_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            material->shadingModel = (int32)Material::MaterialType::SPECULARGLOSSINESS;
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_PBRSPECULARGLOSSINESS_EXTENSION_NAME)->second;
            GetGLTFVec4(ext,  "diffuseFactor",             material->pbrDiffuseFactor);
            GetGLTFFloat(ext, "glossinessFactor",          material->pbrGlossinessFactor);
            GetGLTFVec3(ext,  "specularFactor",            material->pbrSpecularFactor);
            GetGLTFTexId(ext, "diffuseTexture",            material->pbrDiffuseTexture);
            GetGLTFTexId(ext, "specularGlossinessTexture", material->pbrSpecularGlossinessTexture);
        }

        // KHR_texture_transform
        if (tpbr.baseColorTexture.extensions.find(KHR_TEXTURE_TRANSFORM_EXTENSION_NAME) != tpbr.baseColorTexture.extensions.end())
        {
            const auto& ext = tpbr.baseColorTexture.extensions.find(KHR_TEXTURE_TRANSFORM_EXTENSION_NAME)->second;

            Vector2 offset;
            GetGLTFVec2(ext, "offset", offset);

            Vector2 scale;
            GetGLTFVec2(ext, "scale", scale);

            float rotation;
            GetGLTFFloat(ext, "rotation", rotation);

            int texCoord;
            GetGLTFInt(ext, "texCoord", texCoord);

            material->uvTransform.AppendScale(Vector3(scale.x, scale.y, 1.0f));
            material->uvTransform.AppendRotation(rotation, Vector3::ZeroVector);
            material->uvTransform.AppendTranslation(Vector3(offset.x, offset.y, 0.0f));
        }

        // KHR_materials_unlit
        if (gltfMat.extensions.find(KHR_MATERIALS_UNLIT_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            material->unlit = 1;
        }

        // KHR_materials_anisotropy
        if (gltfMat.extensions.find(KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_ANISOTROPY_EXTENSION_NAME)->second;
            GetGLTFFloat(ext, "anisotropy",          material->anisotropy);
            GetGLTFVec3(ext,  "anisotropyDirection", material->anisotropyDirection);
            // GetGLTFTexId(ext, "anisotropyTexture",   material->anisotropyTexture);
        }

        // KHR_materials_clearcoat
        if (gltfMat.extensions.find(KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_CLEARCOAT_EXTENSION_NAME)->second;
            GetGLTFFloat(ext, "clearcoatFactor",           material->clearcoatFactor);
            GetGLTFTexId(ext, "clearcoatTexture",          material->clearcoatTexture);
            GetGLTFFloat(ext, "clearcoatRoughnessFactor",  material->clearcoatRoughness);
            GetGLTFTexId(ext, "clearcoatRoughnessTexture", material->clearcoatRoughnessTexture);
            // GetGLTFTexId(ext, "clearcoatNormalTexture",    material->clearcoatNormalTexture);
        }

        // KHR_materials_sheen
        if (gltfMat.extensions.find(KHR_MATERIALS_SHEEN_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_SHEEN_EXTENSION_NAME)->second;
            /*GetGLTFVec3(ext,  "sheenColorFactor",      material->sheenColorFactor);
            GetGLTFTexId(ext, "sheenColorTexture",     material->sheenColorTexture);
            GetGLTFFloat(ext, "sheenRoughnessFactor",  material->sheenRoughnessFactor);
            GetGLTFTexId(ext, "sheenRoughnessTexture", material->sheenRoughnessTexture);*/
        }

        // KHR_materials_transmission
        if (gltfMat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_TRANSMISSION_EXTENSION_NAME)->second;
            GetGLTFFloat(ext, "transmissionFactor",  material->transmissionFactor);
            GetGLTFTexId(ext, "transmissionTexture", material->transmissionTexture);
        }

        // KHR_materials_ior
        if (gltfMat.extensions.find(KHR_MATERIALS_IOR_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_IOR_EXTENSION_NAME)->second;
            GetGLTFFloat(ext, "ior", material->ior);
        }

        // KHR_materials_volume
        if (gltfMat.extensions.find(KHR_MATERIALS_VOLUME_EXTENSION_NAME) != gltfMat.extensions.end())
        {
            const auto& ext = gltfMat.extensions.find(KHR_MATERIALS_VOLUME_EXTENSION_NAME)->second;
            GetGLTFFloat(ext, "thicknessFactor",     material->thicknessFactor);
            GetGLTFTexId(ext, "thicknessTexture",    material->thicknessTexture);
            GetGLTFFloat(ext, "attenuationDistance", material->attenuationDistance);
            GetGLTFVec3(ext,  "attenuationColor",    material->attenuationColor);
        }
    }
}

LoadGLTFJob::LoadGLTFJob(const std::string& path)
    : m_Path(path)
    , m_Scene3D(nullptr)
{

}

LoadGLTFJob::~LoadGLTFJob()
{

}

void LoadGLTFJob::DoThreadedWork()
{
    bool result = false;
    std::string error;
    std::string warn;
    tinygltf::Model tinyModel;
    tinygltf::TinyGLTF tinyContext;
    std::string extension = GetFileExtension(m_Path);

    if (extension == "gltf")
    {
        result = tinyContext.LoadASCIIFromFile(&tinyModel, &error, &warn, m_Path);
    }
    else
    {
        result = tinyContext.LoadBinaryFromFile(&tinyModel, &error, &warn, m_Path);
    }

    if (result == false)
    {
        m_Scene3D = nullptr;
        return;
    }

    m_Scene3D = std::make_shared<Scene3D>();
    m_Scene3D->numCameras   = int32(tinyModel.cameras.size());
    m_Scene3D->numImages    = int32(tinyModel.images.size());
    m_Scene3D->numTextures  = int32(tinyModel.textures.size());
    m_Scene3D->numMaterials = int32(tinyModel.materials.size());
    m_Scene3D->numSamplers  = int32(tinyModel.samplers.size());
    m_Scene3D->numNodes     = int32(tinyModel.nodes.size());
    m_Scene3D->numMeshes    = int32(tinyModel.meshes.size());
    m_Scene3D->numLights    = int32(tinyModel.lights.size());

    ImportMaterials(m_Scene3D, tinyModel);
}
