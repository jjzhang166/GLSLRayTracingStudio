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
    Please see https://github.com/GPUOpen-LibrariesAndSDKs/RadeonRays_SDK for the original code
    Code is modfied for this project
*/

#include "Math/Math.h"
#include "Math/Bounds3D.h"
#include "Bvh/BvhTranslator.h"

#include <cassert>
#include <stack>
#include <iostream>

int32 BvhTranslator::ProcessBLASNodes(const Bvh::Node* node)
{
    Bounds3D bbox = node->bounds;

    bboxmin[curNode] = bbox.min;
    bboxmax[curNode] = bbox.max;
    nodes[curNode].leaf = 0;

    int32 index = curNode;

    if (node->type == Bvh::NodeType::kLeaf)
    {
        nodes[curNode].leftIndex  = curTriIndex + node->startidx;
        nodes[curNode].rightIndex = node->numprims;
        nodes[curNode].leaf       = 1;
    }
    else
    {
        curNode++;
        nodes[index].leftIndex = ProcessBLASNodes(node->lc);
        nodes[index].leftIndex = ((nodes[index].leftIndex % nodeTexWidth) << 12) | (nodes[index].leftIndex / nodeTexWidth);
        curNode++;
        nodes[index].rightIndex = ProcessBLASNodes(node->rc);
        nodes[index].rightIndex = ((nodes[index].rightIndex % nodeTexWidth) << 12) | (nodes[index].rightIndex / nodeTexWidth);
    }

    return index;
}

int32 BvhTranslator::ProcessTLASNodes(const Bvh::Node* node)
{
    Bounds3D bbox = node->bounds;

    bboxmin[curNode] = bbox.min;
    bboxmax[curNode] = bbox.max;
    nodes[curNode].leaf = 0;

    int32 index = curNode;

    if (node->type == Bvh::NodeType::kLeaf)
    {
        int32 instanceIndex = TLBvh->m_PackedIndices[node->startidx];
        int32 meshIndex     = meshInstances[instanceIndex].meshID;
        int32 materialID    = meshInstances[instanceIndex].materialID;

        nodes[curNode].leftIndex  = (bvhRootStartIndices[meshIndex] % nodeTexWidth) << 12 | (bvhRootStartIndices[meshIndex] / nodeTexWidth);
        nodes[curNode].rightIndex = materialID;
        nodes[curNode].leaf       = -instanceIndex - 1;
    }
    else
    {
        curNode++;
        nodes[index].leftIndex = ProcessTLASNodes(node->lc);
        nodes[index].leftIndex = ((nodes[index].leftIndex % nodeTexWidth) << 12) | (nodes[index].leftIndex / nodeTexWidth);
        curNode++;
        nodes[index].rightIndex = ProcessTLASNodes(node->rc);
        nodes[index].rightIndex = ((nodes[index].rightIndex % nodeTexWidth) << 12) | (nodes[index].rightIndex / nodeTexWidth);
    }

    return index;
}

void BvhTranslator::ProcessBLAS()
{
    int32 nodeCnt = 0;
    for (size_t i = 0; i < meshes.size(); ++i)
    {
        nodeCnt += meshes[i]->bvh->m_Nodecnt;
    }
    
    topLevelIndex = nodeCnt;

    // reserve space for top level nodes
    nodeCnt += 2 * (int32)meshInstances.size();
    nodeTexWidth = (int32)(MMath::Sqrt((float)nodeCnt) + 1);

    // Resize to power of 2
    bboxmin.resize(nodeTexWidth * nodeTexWidth);
    bboxmax.resize(nodeTexWidth * nodeTexWidth);
    nodes.resize(nodeTexWidth * nodeTexWidth);

    curTriIndex = 0;
    int32 bvhRootIndex = 0;
    for (size_t i = 0; i < meshes.size(); i++)
    {
        MeshPtr mesh = meshes[i];
        curNode = bvhRootIndex;

        bvhRootStartIndices.push_back(bvhRootIndex);
        bvhRootIndex += mesh->bvh->m_Nodecnt;
            
        ProcessBLASNodes(mesh->bvh->m_Root);
        curTriIndex += (int32)mesh->bvh->GetNumIndices();
    }
}

void BvhTranslator::ProcessTLAS()
{
    curNode = topLevelIndex;
    topLevelIndexPackedXY = ((topLevelIndex % nodeTexWidth) << 12) | (topLevelIndex / nodeTexWidth);
    ProcessTLASNodes(TLBvh->m_Root);
}

void BvhTranslator::UpdateTLAS(std::shared_ptr<Bvh> topLevelBvh, const std::vector<RendererNode>& sceneInstances)
{
    TLBvh = topLevelBvh;
    curNode = topLevelIndex;
    meshInstances = sceneInstances;
    ProcessTLASNodes(TLBvh->m_Root);
}

void BvhTranslator::Process(std::shared_ptr<Bvh> topLevelBvh, const MeshArray& sceneMeshes, const std::vector<RendererNode>& sceneInstances)
{
    TLBvh = topLevelBvh;
    meshes = sceneMeshes;
    meshInstances = sceneInstances;
    ProcessBLAS();
    ProcessTLAS();
}
