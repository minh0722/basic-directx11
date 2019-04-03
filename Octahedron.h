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

    void triangulate(int level)
    {
        m_indices.clear();
        triangulateHelper(level, 0, 1, 2);
        triangulateHelper(level, 0, 2, 3);
        triangulateHelper(level, 0, 3, 4);
        triangulateHelper(level, 0, 4, 1);

        triangulateHelper(level, 5, 2, 1);
        triangulateHelper(level, 5, 3, 2);
        triangulateHelper(level, 5, 4, 3);
        triangulateHelper(level, 5, 1, 4);
    }

    const std::vector<Vector4f>& GetVertices() { return m_vertices; }
    const std::vector<uint32_t>& GetIndices() { return m_indices; }

private:

    void triangulateHelper(int level, int idx1, int idx2, int idx3)
    {
        if (level == 0)
        {
            m_indices.push_back(idx1);
            m_indices.push_back(idx2);
            m_indices.push_back(idx3);

            return;
        }

        Vector4f x1 = m_vertices[idx1];
        Vector4f x2 = m_vertices[idx2];
        Vector4f x3 = m_vertices[idx3];

        Vector4f x12 = Vector4f((x1.x + x2.x) / 2.0f, (x1.y + x2.y) / 2.0f, (x1.z + x2.z) / 2.0f, 1.0f);
        Vector4f x23 = Vector4f((x2.x + x3.x) / 2.0f, (x2.y + x3.y) / 2.0f, (x2.z + x3.z) / 2.0f, 1.0f);
        Vector4f x31 = Vector4f((x3.x + x1.x) / 2.0f, (x3.y + x1.y) / 2.0f, (x3.z + x1.z) / 2.0f, 1.0f);

        int idx12 = (int)m_vertices.size();
        int idx23 = idx12 + 1;
        int idx31 = idx23 + 1;

        m_vertices.push_back(x12);
        m_vertices.push_back(x23);
        m_vertices.push_back(x31);

        triangulateHelper(level - 1, idx1,  idx12, idx31);
        triangulateHelper(level - 1, idx12, idx2,  idx23);
        triangulateHelper(level - 1, idx31, idx23, idx3 );
        triangulateHelper(level - 1, idx31, idx12, idx23);
    }

private:
    std::vector<Vector4f> m_vertices;
    std::vector<uint32_t> m_indices;
};