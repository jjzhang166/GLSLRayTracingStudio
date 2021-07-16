#include <iostream>

#include "Base/Mesh.h"

void Mesh::BuildBVH()
{
    const int32 numTris = (int32)verticesUVX.size() / 3;
    std::vector<Bounds3D> bounds(numTris);

    for (int32 i = 0; i < numTris; ++i)
    {
        const Vector3 v1 = Vector3(verticesUVX[i * 3 + 0]);
        const Vector3 v2 = Vector3(verticesUVX[i * 3 + 1]);
        const Vector3 v3 = Vector3(verticesUVX[i * 3 + 2]);

        bounds[i].Expand(v1);
        bounds[i].Expand(v2);
        bounds[i].Expand(v3);
    }

    bvh->Build(&bounds[0], numTris);
}
