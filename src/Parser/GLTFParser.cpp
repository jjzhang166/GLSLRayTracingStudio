﻿#include "Parser/GLTFParser.h"
#include "Parser/tiny_gltf.h"

#include "Misc/FileMisc.h"
#include "Base/Base.h"

#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Quat.h"

#include <string>
#include <glad/glad.h>

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
    std::vector<T> result;
    if (!value.IsArray())
    {
        return result;
    }

    result.resize(value.ArrayLen());
    for (int32 i = 0; i < value.ArrayLen(); i++)
    {
        result[i] = static_cast<T>(value.Get(i).IsNumber() ? value.Get(i).Get<double>() : value.Get(i).Get<int32>());
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

static FORCEINLINE void GetGLTFInt(const tinygltf::Value& value, const std::string& name, int32& val)
{
    if (value.Has(name))
    {
        val = value.Get(name).Get<int32>();
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

static FORCEINLINE void GetGLTFTexId(const tinygltf::Value& value, const std::string& name, int32& val)
{
    if (value.Has(name))
    {
        val = value.Get(name).Get("index").Get<int32>();
    }
}

template <typename T>
static bool GetGLTFAttribute(const tinygltf::Model& model, const tinygltf::Primitive& primitive, std::vector<T>& attribVec, const std::string& attribName)
{
    if (primitive.attributes.find(attribName) == primitive.attributes.end())
    {
        return false;
    }
    
    const auto& accessor = model.accessors[primitive.attributes.find(attribName)->second];
    const auto& bufView  = model.bufferViews[accessor.bufferView];
    const auto& buffer   = model.buffers[bufView.buffer];
    const auto  bufData  = reinterpret_cast<const T*>(&(buffer.data[accessor.byteOffset + bufView.byteOffset]));
    const auto  numElems = accessor.count;

    if (accessor.componentType == TINYGLTF_COMPONENT_TYPE_FLOAT)
    {
        if (bufView.byteStride == 0)
        {
            attribVec.insert(attribVec.end(), bufData, bufData + numElems);
        }
        else
        {
            auto bufferByte = reinterpret_cast<const uint8*>(bufData);
            for (size_t i = 0; i < numElems; ++i)
            {
                attribVec.push_back(*reinterpret_cast<const T*>(bufferByte));
                bufferByte += bufView.byteStride;
            }
        }
    }
    else
    {
        int32 numComps      = accessor.type == TINYGLTF_TYPE_VEC2 ? 2 : (accessor.type == TINYGLTF_TYPE_VEC3) ? 3 : 4;
        size_t strideComp = accessor.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? 1 : 2;
        size_t byteStride = bufView.byteStride > 0 ? bufView.byteStride : size_t(numComps) * strideComp;
        auto   bufferByte = reinterpret_cast<const uint8*>(bufData);

        for (size_t i = 0; i < numElems; ++i)
        {
            T vecValue;
            auto bufferByteData = bufferByte;
            for (int32 c = 0; c < numComps; ++c)
            {
                float value = *reinterpret_cast<const float*>(bufferByteData);
                switch (accessor.componentType)
                {
                    case TINYGLTF_COMPONENT_TYPE_BYTE:
                    {
                        vecValue[c] = MMath::Max(value / 127.f, -1.f);
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
                    {
                        vecValue[c] = value / 255.f;
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_SHORT:
                    {
                        vecValue[c] = MMath::Max(value / 32767.f, -1.f);
                        break;
                    }
                    case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
                    {
                        vecValue[c] = value / 65535.f;
                        break;
                    }
                    default:
                    {
                        break;
                    }
                }
                bufferByteData += strideComp;
            }
            bufferByte += byteStride;
            attribVec.push_back(vecValue);
        }
    }

    return true;
}

static void ImportMaterials(Scene3DPtr scene, tinygltf::Model& model)
{
    for (int32 i = 0; i < (int32)model.materials.size(); ++i)
    {
        auto& gltfMat = model.materials[i];
        auto material = std::make_shared<Material>();
        scene->materials.push_back(material);

        // base
        material->alphaCutoff        = float(gltfMat.alphaCutoff);
        material->alphaMode          = gltfMat.alphaMode == "MASK" ? (int32)Material::AlphaType::MASK : (gltfMat.alphaMode == "BLEND" ? (int32)Material::AlphaType::BLEND : (int32)Material::AlphaType::NONE);
        material->doubleSided        = gltfMat.doubleSided ? 1 : 0;
        material->emissiveFactor     = Vector3((float)gltfMat.emissiveFactor[0], (float)gltfMat.emissiveFactor[1], (float)gltfMat.emissiveFactor[2]);
        material->emissiveTexture    = gltfMat.emissiveTexture.index;
        material->normalTexture      = gltfMat.normalTexture.index;
        material->normalTextureScale = float(gltfMat.normalTexture.scale);
        // Raytracing don't need this.
        // gltfMat.occlusionTexture.index;
        // gltfMat.occlusionTexture.strength;

        // PbrMetallicRoughness
        auto& tpbr = gltfMat.pbrMetallicRoughness;
        material->shadingModel                  = (int32)Material::MaterialType::METALLICROUGHNESS;
        material->pbrBaseColorFactor            = Vector4((float)tpbr.baseColorFactor[0], (float)tpbr.baseColorFactor[1], (float)tpbr.baseColorFactor[2], (float)tpbr.baseColorFactor[3]);
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

            int32 texCoord;
            GetGLTFInt(ext, "texCoord", texCoord);

            material->offsetScale = Vector4(offset.x, offset.y, scale.x, scale.y);
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

    if (scene->materials.empty())
    {
        auto material = std::make_shared<Material>();
        scene->materials.push_back(material);
        material->pbrMetallicFactor = 0.0f;
    }
}

static void ImportMesh(Scene3DPtr scene, tinygltf::Model& model, Object3DPtr object3D, int32 gltfMeshID)
{
    auto& gltfMesh = model.meshes[gltfMeshID];
    for (int32 gltfPrimIdx = 0; gltfPrimIdx < (int32)gltfMesh.primitives.size(); ++gltfPrimIdx)
    {
        auto& gltfPrim = gltfMesh.primitives[gltfPrimIdx];
        MeshPtr mesh   = std::make_shared<Mesh>();
        mesh->material = MMath::Max(0, gltfPrim.material);
        mesh->node     = object3D;

        // add to scene
        {
            scene->meshes.push_back(mesh);
            object3D->meshes.push_back(mesh);
            object3D->materials.push_back(scene->materials[mesh->material]);
        }

        if (gltfMesh.primitives.size() == 1)
        {
            mesh->name = gltfMesh.name;
        }
        else
        {
            mesh->name = std::string(gltfMesh.name) + "_primitive" + std::to_string(gltfPrimIdx);
        }

        // indices
        if (gltfPrim.indices > -1)
        {
            const tinygltf::Accessor&   indexAccessor = model.accessors[gltfPrim.indices];
            const tinygltf::BufferView& bufferView    = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer&     buffer        = model.buffers[bufferView.buffer];

            mesh->indices.resize(indexAccessor.count);

            if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT)
            {
                memcpy(mesh->indices.data(), &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(uint32));
            }
            else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT)
            {
                std::vector<uint16> temp;
                temp.resize(indexAccessor.count);
                memcpy(temp.data(), &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(uint16));
                for (int32 idx = 0; idx < (int32)temp.size(); ++idx)
                {
                    mesh->indices[idx] = temp[idx];
                }
            }
            else if (indexAccessor.componentType == TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE)
            {
                std::vector<uint8> temp;
                temp.resize(indexAccessor.count);
                memcpy(temp.data(), &buffer.data[indexAccessor.byteOffset + bufferView.byteOffset], indexAccessor.count * sizeof(uint8));
                for (int32 idx = 0; idx < (int32)temp.size(); ++idx)
                {
                    mesh->indices[idx] = temp[idx];
                }
            }
        }
        else
        {
            const auto& accessor = model.accessors[gltfPrim.attributes.find("POSITION")->second];
            mesh->indices.resize(accessor.count);
            for (uint32 i = 0; i < (uint32)accessor.count; ++i)
            {
                mesh->indices[i] = i;
            }
        }

        // position
        {
            GetGLTFAttribute<Vector3>(model, gltfPrim, mesh->positions, "POSITION");

            const auto& accessor = model.accessors[gltfPrim.attributes.find("POSITION")->second];
            if (accessor.minValues.size() == 3)
            {
                mesh->aabb.min = Vector3((float)accessor.minValues[0], (float)accessor.minValues[1], (float)accessor.minValues[2]);
            }
            if (accessor.maxValues.size() == 3)
            {
                mesh->aabb.max = Vector3((float)accessor.maxValues[0], (float)accessor.maxValues[1], (float)accessor.maxValues[2]);
            }
        }

        // normal
        {
            std::vector<Vector3> normals;
            if (!GetGLTFAttribute<Vector3>(model, gltfPrim, normals, "NORMAL"))
            {
                normals.resize(mesh->positions.size());
                for (size_t i = 0; i < mesh->indices.size(); i += 3)
                {
                    uint32 idx0 = mesh->indices[i + 0];
                    uint32 idx1 = mesh->indices[i + 1];
                    uint32 idx2 = mesh->indices[i + 2];
                    const auto& pos0 = mesh->positions[idx0];
                    const auto& pos1 = mesh->positions[idx1];
                    const auto& pos2 = mesh->positions[idx2];
                    const auto v1 = (pos1 - pos0).GetSafeNormal();
                    const auto v2 = (pos2 - pos0).GetSafeNormal();
                    const auto n  = Vector3::CrossProduct(v2, v1);
                    normals[idx0] += n;
                    normals[idx1] += n;
                    normals[idx2] += n;
                }

                for (size_t i = 0; i < normals.size(); ++i)
                {
                    normals[i].Normalize();
                }
            }

            mesh->normals.resize(normals.size());
            for (size_t i = 0; i < normals.size(); ++i)
            {
                // pow(2,10) = 1024
                Vector3 normal = (normals[i] + 1.0f) * 0.5f;
                uint8 r = uint8(normal.x * 1024);
                uint8 g = uint8(normal.y * 1024);
                uint8 b = uint8(normal.z * 1024);
                uint32 val = (r << 20) | (g << 10) | (b);
                mesh->normals[i] = val;
                // uint16 r0 = (val >> 20) & 1023;
                // uint16 g1 = (val >> 10) & 1023;
                // uint16 b1 = (val >>  0) & 1023;
            }
        }

        // uv
        {
            if (!GetGLTFAttribute<Vector2>(model, gltfPrim, mesh->uvs, "TEXCOORD_0"))
            {
                mesh->uvs.resize(mesh->positions.size());
                for (size_t i = 0; i < mesh->positions.size(); ++i)
                {
                    mesh->uvs[i].x = 0.0f;
                    mesh->uvs[i].y = 0.0f;
                }
            }
        }

        // tangent
        {
            std::vector<Vector4> tangents;
            if (!GetGLTFAttribute<Vector4>(model, gltfPrim, tangents, "TANGENT"))
            {
                std::vector<Vector3> tempTangents;
                std::vector<Vector3> tempBitangents;
                tempTangents.resize(mesh->positions.size());
                tempBitangents.resize(mesh->positions.size());

                for (size_t i = 0; i < mesh->indices.size(); i += 3)
                {
                    uint32 idx0 = mesh->indices[i + 0];
                    uint32 idx1 = mesh->indices[i + 1];
                    uint32 idx2 = mesh->indices[i + 2];

                    const auto& p0 = mesh->positions[idx0];
                    const auto& p1 = mesh->positions[idx0];
                    const auto& p2 = mesh->positions[idx0];

                    const auto& uv0 = mesh->uvs[idx0];
                    const auto& uv1 = mesh->uvs[idx0];
                    const auto& uv2 = mesh->uvs[idx0];

                    Vector3 e1 = p1 - p0;
                    Vector3 e2 = p2 - p0;

                    Vector2 duvE1 = uv1 - uv0;
                    Vector2 duvE2 = uv2 - uv0;

                    float r = 1.0f;
                    float a = duvE1.x * duvE2.y - duvE2.x * duvE1.y;
                    if (MMath::Abs(a) > 0.0f)
                    {
                        r = 1.0f / a;
                    }

                    Vector3 t = (e1 * duvE2.y - e2 * duvE1.y) * r;
                    Vector3 b = (e2 * duvE1.x - e1 * duvE2.x) * r;

                    tempTangents[idx0] += t;
                    tempTangents[idx1] += t;
                    tempTangents[idx2] += t;

                    tempBitangents[idx0] += b;
                    tempBitangents[idx1] += b;
                    tempBitangents[idx2] += b;
                }

                tangents.resize(mesh->positions.size());
                for (size_t i = 0; i < mesh->positions.size(); ++i)
                {
                    const auto& normal = mesh->normals[i];
                    uint16 red   = (normal >> 20) & 1023;
                    uint16 green = (normal >> 10) & 1023;
                    uint16 blue  = (normal >>  0) & 1023;
                    Vector3 n    = Vector3(red / 1024.0f, green / 1024.0f, blue / 1024.0f) * 2.0f - 1.0f;

                    const auto& t = tempTangents[i];
                    const auto& b = tempBitangents[i];

                    Vector3 tangent  = (t - (Vector3::DotProduct(n, t) * n)).GetSafeNormal();
                    float handedness = (Vector3::DotProduct(Vector3::CrossProduct(n, t), b) < 0.0f) ? -1.0f : 1.0f;

                    tangents[i] = Vector4(tangent, handedness);
                }
            }

            mesh->tangents.resize(tangents.size());
            for (size_t i = 0; i < tangents.size(); ++i)
            {
                Vector3 tangent = (tangents[i] + Vector4(1.0f, 1.0f, 1.0f, 1.0f)) * 0.5f;
                uint8 r = uint8(tangent.x * 1024);
                uint8 g = uint8(tangent.y * 1024);
                uint8 b = uint8(tangent.z * 1024);
                uint8 a = tangents[i].w > 0.0f ? 1 : 0;
                uint32 val = (a << 30) | (r << 20) | (g << 10) | (b);
                mesh->tangents[i] = val;
            }
        }

        // color
        {
            std::vector<Vector4> colors;
            if (!GetGLTFAttribute<Vector4>(model, gltfPrim, colors, "COLOR_0"))
            {
                colors.resize(mesh->positions.size());
                for (size_t i = 0; i < mesh->positions.size(); ++i)
                {
                    colors[i] = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
                }
            }
        }

        // bvh
        {
            mesh->BuildBVH();
        }
    }
}

static void ImportCamera(Scene3DPtr scene, tinygltf::Model& model, Object3DPtr object3D, tinygltf::Node& gltfNode)
{
    const auto& gltfCamera = model.cameras[gltfNode.camera];
    
    // perspective
    if (gltfCamera.type == "perspective")
    {
        std::shared_ptr<Camera> camera = std::make_shared<Camera>();
        
        // add to scene
        {
            object3D->camera = camera;
            scene->cameras.push_back(camera);
        }

        camera->node = object3D;
        camera->Perspective((float)gltfCamera.perspective.yfov, 1.0f, 1.0f, (float)gltfCamera.perspective.znear, (float)gltfCamera.perspective.zfar);
    }
    else
    {
        // not support
    }
}

static void ImportLight(Scene3DPtr scene, tinygltf::Model& model, Object3DPtr object3D, tinygltf::Node& gltfNode)
{
    auto& extension = gltfNode.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME)->second;
    auto lightIdx   = extension.Get("light").GetNumberAsInt();
    auto& gltfLight = model.lights[lightIdx];
    LightPtr light  = std::make_shared<Light>();

    // add to scene
    {
        object3D->light = light;
        scene->lights.push_back(light);
    }

    light->node      = object3D;
    light->position  = object3D->GlobalTransform().GetOrigin();
    light->direction = object3D->GlobalTransform().GetForward();
    light->color     = Vector3((float)gltfLight.color[0], (float)gltfLight.color[1], (float)gltfLight.color[2]);
    light->innerCone = (float)gltfLight.spot.innerConeAngle;
    light->outerCone = (float)gltfLight.spot.outerConeAngle;
    light->range     = (float)gltfLight.range;
    light->intensity = (float)gltfLight.intensity;
    if (gltfLight.type == "point")
    {
        light->type = Light::LightType::POINT;
    }
    else if (gltfLight.type == "directional")
    {
        light->type = Light::LightType::DIRECTIONAL;
    }
    else if (gltfLight.type == "spot")
    {
        light->type = Light::LightType::SPOT;
    }
}

static void ImportNode(Scene3DPtr scene, tinygltf::Model& model, int32 nodeID, Object3DPtr parent)
{
    auto& gltfNode = model.nodes[nodeID];
    auto object3D  = std::make_shared<Object3D>();

    // add to scene
    {
        scene->nodes.push_back(object3D);
    }

    // name
    object3D->name = gltfNode.name;

    if (parent)
    {
        object3D->parent = parent;
        parent->children.push_back(object3D);
    }

    // transform
    {
        object3D->transform.SetIdentity();
        if (gltfNode.rotation.size() == 4) 
        {
            Quat quat((float)gltfNode.rotation[0], (float)gltfNode.rotation[1], (float)gltfNode.rotation[2], (float)gltfNode.rotation[3]);
            object3D->transform.Append(quat.ToMatrix());
        }
        if (gltfNode.scale.size() == 3) 
        {
            object3D->transform.AppendScale(Vector3((float)gltfNode.scale[0], (float)gltfNode.scale[1], (float)gltfNode.scale[2]));
        }
        if (gltfNode.translation.size() == 3) 
        {
            object3D->transform.AppendTranslation(Vector3((float)gltfNode.translation[0], (float)gltfNode.translation[1], (float)gltfNode.translation[2]));
        }
    }

    // mesh
    if (gltfNode.mesh > -1)
    {
        ImportMesh(scene, model, object3D, gltfNode.mesh);
    }
    // camera
    else if (gltfNode.camera > -1)
    {
        ImportCamera(scene, model, object3D, gltfNode);
    }
    // light
    else if (gltfNode.extensions.find(KHR_LIGHTS_PUNCTUAL_EXTENSION_NAME) != gltfNode.extensions.end())
    {
        ImportLight(scene, model, object3D, gltfNode);
    }

    // children
    for (size_t i = 0; i < gltfNode.children.size(); ++i)
    {
        ImportNode(scene, model, gltfNode.children[i], object3D);
    }
}

static void ImportNodes(Scene3DPtr scene, tinygltf::Model& model)
{
    const auto& gltfScene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

    // root node
    scene->rootNode = std::make_shared<Object3D>();
    scene->rootNode->name = "RootNode";
    scene->rootNode->transform.SetIdentity();

    // add to scene
    {
        scene->nodes.push_back(scene->rootNode);
    }

    for (size_t idx = 0; idx < gltfScene.nodes.size(); ++idx)
    {
        int32 nodeID = gltfScene.nodes[idx];
        ImportNode(scene, model, nodeID, scene->rootNode);
    }
}

static void ImportImages(Scene3DPtr scene, tinygltf::Model& model)
{
    for (int32 i = 0; i < (int32)model.images.size(); ++i)
    {
        const auto& gltfImage = model.images[i];
        std::shared_ptr<Image> image = std::make_shared<Image>();

        // add to scene
        {
            scene->images.push_back(image);
        }

        image->width  = gltfImage.width;
        image->height = gltfImage.height;
        image->comp   = gltfImage.component;
        image->rgba.resize(gltfImage.image.size());
        memcpy(image->rgba.data(), gltfImage.image.data(), gltfImage.image.size());
    }
}

static void ImportTextures(Scene3DPtr scene, tinygltf::Model& model)
{
    std::map<int32, int32> filters;
    filters[9728] = GL_NEAREST;  // NEAREST
    filters[9729] = GL_LINEAR;   // LINEAR
    filters[9984] = GL_NEAREST;  // NEAREST_MIPMAP_NEAREST
    filters[9985] = GL_LINEAR;   // LINEAR_MIPMAP_NEAREST
    filters[9986] = GL_NEAREST;  // NEAREST_MIPMAP_LINEAR
    filters[9987] = GL_LINEAR;   // LINEAR_MIPMAP_LINEAR
    
    std::map<int32, int32> mipmap;
    mipmap[9728] = GL_NEAREST;  // NEAREST
    mipmap[9729] = GL_LINEAR;  // LINEAR
    mipmap[9984] = GL_NEAREST_MIPMAP_NEAREST;  // NEAREST_MIPMAP_NEAREST
    mipmap[9985] = GL_LINEAR_MIPMAP_NEAREST;  // LINEAR_MIPMAP_NEAREST
    mipmap[9986] = GL_NEAREST_MIPMAP_LINEAR;   // NEAREST_MIPMAP_LINEAR
    mipmap[9987] = GL_LINEAR_MIPMAP_LINEAR;   // LINEAR_MIPMAP_LINEAR

    std::map<int32, int32> addressMode;
    addressMode[33071] = GL_CLAMP_TO_EDGE;
    addressMode[33648] = GL_MIRRORED_REPEAT;
    addressMode[10497] = GL_REPEAT;

    for (int32 i = 0; i < (int32)model.textures.size(); ++i)
    {
        const auto& gltfTexture = model.textures[i];
        const auto& gltfSampler = model.samplers[gltfTexture.sampler];
        std::shared_ptr<Texture> texture = std::make_shared<Texture>();

        // add to scene
        {
            scene->textures.push_back(texture);
        }

        texture->source    = scene->images[gltfTexture.source];
        texture->minFilter = filters[gltfSampler.minFilter];
        texture->magFilter = filters[gltfSampler.magFilter];
        texture->wrapS     = addressMode[gltfSampler.wrapS];
        texture->wrapT     = addressMode[gltfSampler.wrapT];
    }
}

static void CalcSceneDimensions(Scene3DPtr scene)
{
    for (size_t i = 0; i < scene->nodes.size(); ++i)
    {
        Object3DPtr node = scene->nodes[i];
        if (node->meshes.size() == 0)
        {
            continue;
        }

        for (size_t m = 0; m < node->meshes.size(); ++m)
        {
            auto mesh  = node->meshes[m];
            auto world = node->GlobalTransform();
            
            Vector3 corners[8];
            corners[0].Set(mesh->aabb.min.x, mesh->aabb.min.y, mesh->aabb.min.z);
            corners[1].Set(mesh->aabb.max.x, mesh->aabb.min.y, mesh->aabb.min.z);
            corners[2].Set(mesh->aabb.min.x, mesh->aabb.max.y, mesh->aabb.min.z);
            corners[3].Set(mesh->aabb.max.x, mesh->aabb.max.y, mesh->aabb.min.z);
            corners[4].Set(mesh->aabb.min.x, mesh->aabb.min.y, mesh->aabb.max.z);
            corners[5].Set(mesh->aabb.max.x, mesh->aabb.min.y, mesh->aabb.max.z);
            corners[6].Set(mesh->aabb.min.x, mesh->aabb.max.y, mesh->aabb.max.z);
            corners[7].Set(mesh->aabb.max.x, mesh->aabb.max.y, mesh->aabb.max.z);

            for (int32 b = 0; b < 8; ++b)
            {
                Vector3 vec = world.TransformPosition(corners[b]);
                scene->bounds.min = Vector3::Min(scene->bounds.min, vec);
                scene->bounds.max = Vector3::Max(scene->bounds.max, vec);
            }
        }
    }
}

static void FitSceneCamera(Scene3DPtr scene)
{
    if (scene->cameras.size() != 0)
    {
        return;
    }

    CameraPtr camera = std::make_shared<Camera>();
    camera->Perspective(MMath::DegreesToRadians(60.0f), 1.0f, 1.0f, 0.1f, 3000.0f);

    // add to scene
    Object3DPtr node = std::make_shared<Object3D>();
    node->name   = "DefaultCamera";
    node->camera = camera;
    node->parent = scene->rootNode;
    camera->node = node;

    scene->rootNode->children.push_back(node);
    scene->nodes.push_back(node);
    scene->cameras.push_back(camera);

    // update position
    Vector3 center = scene->bounds.Center();
    Vector3 extent = scene->bounds.Extents();
    Vector3 eye    = Vector3(center.x, center.y, center.z - extent.Size() * 1.0f);
    Vector3 at     = center;
    node->transform.SetPosition(eye);
    node->transform.LookAt(center);
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

    ImportMaterials(m_Scene3D, tinyModel);
    ImportNodes(m_Scene3D, tinyModel);
    ImportImages(m_Scene3D, tinyModel);
    ImportTextures(m_Scene3D, tinyModel);
    CalcSceneDimensions(m_Scene3D);
    FitSceneCamera(m_Scene3D);
}
