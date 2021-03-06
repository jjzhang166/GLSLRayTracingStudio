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
#pragma once

#include <vector>

#include "Math/Bounds3D.h"

class Bvh
{
public:
    Bvh(float traversalCost, int32 numBins = 64, bool usesah = false)
        : m_Root(nullptr)
        , m_Usesah(usesah)
        , m_Height(0)
        , m_TraversalCost(traversalCost)
        , m_NumBins(numBins)
    {
            
    }

    virtual ~Bvh()
    {
            
    }

    // Build function
    // bounds is an array of bounding boxes
    void Build(const Bounds3D* bounds, int32 numbounds);

    // World space bounding box
    const Bounds3D& Bounds() const
    {
        return m_Bounds;
    }

    // Get tree height
    int32 GetHeight() const
    {
        return m_Height;
    }

    // Get reordered prim indices Nodes are pointing to
    virtual const int32* GetIndices() const
    {
        return &m_PackedIndices[0];
    }

    // Get number of indices.
    // This number can differ from numbounds passed to Build function for
    // some BVH implementations (like SBVH)
    virtual size_t GetNumIndices() const
    {
        return m_PackedIndices.size();
    }

protected:

    // Enum for node type
    enum NodeType
    {
        kInternal,
        kLeaf
    };

    // BVH node
    struct Node
    {
        // Node bounds in world space
        Bounds3D bounds;
        // Type of the node
        NodeType type;
        // Node index in a complete tree
        int32 index;

        union
        {
            // For internal nodes: left and right children
            struct
            {
                Node* lc;
                Node* rc;
            };

            // For leaves: starting primitive index and number of primitives
            struct
            {
                int32 startidx;
                int32 numprims;
            };
        };
    };

    struct SplitRequest
    {
        // Starting index of a request
        int32 startidx;
        // Number of primitives
        int32 numprims;
        // Root node
        Node** ptr;
        // Bounding box
        Bounds3D bounds;
        // Centroid bounds
        Bounds3D centroidBounds;
        // Level
        int32 level;
        // Node index
        int32 index;
    };

    struct SahSplit
    {
        int32 dim;
        float split;
        float sah;
        float overlap;
    };

protected:

    // Build function
    virtual void BuildImpl(const Bounds3D* bounds, int32 numbounds);

    // Node allocation
    virtual Node* AllocateNode();

    virtual void InitNodeAllocator(size_t maxnum);

    void BuildNode(const SplitRequest& req, const Bounds3D* bounds, const Vector3* centroids, int32* primindices);

    SahSplit FindSahSplit(const SplitRequest& req, const Bounds3D* bounds, const Vector3* centroids, int32* primindices) const;

    // Bvh nodes
    std::vector<Node> m_Nodes;
    // Identifiers of leaf primitives
    std::vector<int32> m_Indices;
    // Node allocator counter, atomic for thread safety
    int32 m_Nodecnt;
    // Identifiers of leaf primitives
    std::vector<int32> m_PackedIndices;
    // Bounding box containing all primitives
    Bounds3D m_Bounds;
    // Root node
    Node* m_Root;
    // SAH flag
    bool m_Usesah;
    // Tree height
    int32 m_Height;
    // Node traversal cost
    float m_TraversalCost;
    // Number of spatial bins to use for SAH
    int32 m_NumBins;

private:

    Bvh(const Bvh& bvh) = delete;

    Bvh& operator = (const Bvh& bvh) = delete;

    friend class BvhTranslator;
};
