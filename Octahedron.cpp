#include "pch.h"
#include "Octahedron.h"

static std::vector<Vector4f> checker;

Octahedron::Octahedron(float radius)
    : m_radius(radius)
{
    // top
    m_vertices.push_back(Vector4f(0.0f, radius * 1.0f, 0.0f, 1.0f));

    // 4 side vertices
    m_vertices.push_back(Vector4f(radius * 1.0f, 0.0f,  0.0f, 1.0f));
    m_vertices.push_back(Vector4f( 0.0f, 0.0f, radius * 1.0f, 1.0f));
    m_vertices.push_back(Vector4f(radius * -1.0f, 0.0f,  0.0f, 1.0f));
    m_vertices.push_back(Vector4f( 0.0f, 0.0f, radius * -1.0f, 1.0f));
    
    // bottom
    m_vertices.push_back(Vector4f(0.0f, radius * -1.0f, 0.0f, 1.0f));
}


void Octahedron::triangulate(int level)
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

void Octahedron::triangulateHelper(int level, int idx1, int idx2, int idx3)
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

    // normalize so vector has length 1
    x12.Normalize3();
    x23.Normalize3();
    x31.Normalize3();

    // project all points to sphere by multiply by radius
    x12.Mul3(Vector4f(m_radius, m_radius, m_radius, 1.0f));
    x23.Mul3(Vector4f(m_radius, m_radius, m_radius, 1.0f));
    x31.Mul3(Vector4f(m_radius, m_radius, m_radius, 1.0f));

    int idx12 = (int)m_vertices.size();
    int idx23 = idx12 + 1;
    int idx31 = idx23 + 1;

    if (std::find(checker.begin(), checker.end(), x12) == checker.end()) checker.push_back(x12);
    if (std::find(checker.begin(), checker.end(), x23) == checker.end()) checker.push_back(x23);
    if (std::find(checker.begin(), checker.end(), x31) == checker.end()) checker.push_back(x31);

    m_vertices.push_back(x12);
    m_vertices.push_back(x23);
    m_vertices.push_back(x31);

    triangulateHelper(level - 1, idx1, idx12, idx31);
    triangulateHelper(level - 1, idx12, idx2, idx23);
    triangulateHelper(level - 1, idx31, idx23, idx3);
    triangulateHelper(level - 1, idx31, idx12, idx23);
}

const std::vector<Vector4f>& Octahedron::GetVertices() 
{ 
    return m_vertices; 
}

const std::vector<uint32_t>& Octahedron::GetIndices() 
{
    return m_indices; 
}