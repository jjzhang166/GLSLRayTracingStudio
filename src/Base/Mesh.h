#pragma once

#include <vector>

#include "Math/Vector3.h"
#include "Math/Vector4.h"
#include "Math/Matrix4x4.h"
#include "Bvh/SplitBvh.h"

class Mesh
{
public:
    Mesh()
        : bvh(nullptr)
    { 
        bvh = new SplitBvh(2.0f, 64, 0, 0.001f, 2.5f); 
    }

    ~Mesh()
    { 
        if (bvh)
        {
            delete bvh;
            bvh = nullptr;
        }
    }
    
    void BuildBVH();

public:

    Bvh*                    bvh;
    std::string             name;

    // Mesh Data
    std::vector<Vector4>    verticesUVX;
    std::vector<Vector4>    normalsUVY;
};

class MeshInstance
{
public:
    MeshInstance(int meshId, Matrix4x4 xform, int matId, const std::string& inName)
        : transform(xform)
        , materialID(matId)
        , meshID(meshId)
        , name(inName)
    {

    }

    ~MeshInstance() 
    {

    }

public:
    Matrix4x4	    transform;
    int			    materialID;
    int			    meshID;
    std::string     name;
};
