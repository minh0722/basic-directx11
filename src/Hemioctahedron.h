#pragma once

#include "OctahedronVertex.h"

class Hemioctahedron
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
    Hemioctahedron(float radius, uint32_t subdivisionLevel = 0);

    void triangulate(int subdivisionLevel);
    const std::vector<uint32_t>& GetIndices();

    template <typename Functor>
    void ProcessVertices(Functor f)
    {
        for (uint32_t i = 0; i < m_vertices.size(); ++i)
            f(m_vertices[i] *= m_radius, i);
    }

private:
    void triangulateHelper(int level, int idx1, int idx2, int idx3);
    void triangulateLastLevel(int idx1, int idx2, int idx3);

private:
    std::vector<OctahedronVertex> m_vertices;
    std::vector<uint32_t> m_indices;
    uint32_t m_subdivisionLevel;
    float m_radius;
};