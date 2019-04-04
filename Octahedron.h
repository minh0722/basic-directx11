#pragma once
#include "Vector4f.h"

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

    const std::vector<Vector4f>& GetVertices();
    const std::vector<uint32_t>& GetIndices();

private:
    void triangulateHelper(int level, int idx1, int idx2, int idx3);

private:
    std::vector<Vector4f> m_vertices;
    std::vector<uint32_t> m_indices;
    float m_radius;
};