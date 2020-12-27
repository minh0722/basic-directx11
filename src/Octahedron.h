#pragma once
#include "OctahedronVertex.h"

class Octahedron
{
    /*
            x1
            /\
           /  \
          /    \
         /      \
        /________\
       x2        x3
    */

public:
    Octahedron(float radius);

    void triangulate(int level);

    const std::vector<OctahedronVertex>& GetVertices();
    const std::vector<uint32_t>& GetIndices();

private:
    void triangulateHelper(int level, int idx1, int idx2, int idx3);

private:
    std::vector<OctahedronVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    float m_radius;
};