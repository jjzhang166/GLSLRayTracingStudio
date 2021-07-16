/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/

/* 
    Code is not the original code from RadeonRays. It is modfied slightly for this project 
    Please refer to https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK for the original code
*/

#pragma once

#include <map>

#include "Bvh.h"
#include "Base/Mesh.h"

/// This class translates pointer based BVH representation into
/// index based one suitable for feeding to GPU or any other accelerator
//
class BvhTranslator
{
public:

    // Constructor
    BvhTranslator() = default;

    struct Node
    {
        int32 leftIndex;
        int32 rightIndex;
        int32 leaf;
    };

    void ProcessBLAS();
    void ProcessTLAS();
    void UpdateTLAS(const Bvh* topLevelBvh, const std::vector<MeshInstance>& instances);
    void Process(const Bvh* topLevelBvh, const std::vector<Mesh*>& meshes, const std::vector<MeshInstance>& instances);
    
private:
    int32 ProcessBLASNodes(const Bvh::Node* root);
    int32 ProcessTLASNodes(const Bvh::Node* root);

public:
    std::vector<Vector3> bboxmin;
    std::vector<Vector3> bboxmax;
    std::vector<Node> nodes;
    int32 nodeTexWidth;
    int32 topLevelIndexPackedXY = 0;
    int32 topLevelIndex = 0;

private:
    int32 curNode = 0;
    int32 curTriIndex = 0;
    const Bvh* TLBvh;
    std::vector<int32> bvhRootStartIndices;
    std::vector<MeshInstance> meshInstances;
    std::vector<Mesh*> meshes;
};
