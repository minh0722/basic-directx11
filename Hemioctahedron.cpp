#include "pch.h"
#include "Hemioctahedron.h"

Hemioctahedron::Hemioctahedron(float radius, uint32_t subdivisionLevel/* = 0*/)
    : m_radius(radius),
    m_subdivisionLevel(subdivisionLevel)
{
    Vector4f x0 = Vector4f(0.0f, m_radius, 0.0f, 1.0f);
    Vector4f x1 = Vector4f(m_radius, 0.0f, 0.0f, 1.0f);
    Vector4f x2 = Vector4f(0.0f, 0.0f, m_radius, 1.0f);
    Vector4f x3 = Vector4f(-m_radius, 0.0f, 0.0f, 1.0f);
    Vector4f x4 = Vector4f(0.0f, 0.0f, -m_radius, 1.0f);

    // top
    m_vertices.push_back({ x0, CalculateUV(x0)});

    // 4 side vertices
    m_vertices.push_back({ x1, CalculateUV(x1)});
    m_vertices.push_back({ x2, CalculateUV(x2)});
    m_vertices.push_back({ x3, CalculateUV(x3)});
    m_vertices.push_back({ x4, CalculateUV(x4)});

    if (subdivisionLevel != 0)
        triangulate(subdivisionLevel);
}

void Hemioctahedron::triangulate(int subdivisionLevel)
{
    m_subdivisionLevel = subdivisionLevel;

    m_indices.clear();
    
    // erase all vertices except the base ones
    m_vertices.erase(m_vertices.begin() + 5, m_vertices.end());

    triangulateHelper(subdivisionLevel, 0, 1, 2);
    triangulateHelper(subdivisionLevel, 0, 2, 3);
    triangulateHelper(subdivisionLevel, 0, 3, 4);
    triangulateHelper(subdivisionLevel, 0, 4, 1);
}

void Hemioctahedron::triangulateHelper(int level, int idx1, int idx2, int idx3)
{
    if (level == 0)
    {
        if (idx1 == 0 || idx2 == 0 || idx3 == 0)
            return;

        //m_indices.push_back(idx1);
        //m_indices.push_back(idx2);
        //m_indices.push_back(idx3);

        triangulateLastLevel(idx1, idx2, idx3);

        return;
    }

    const Vector4f& x1 = m_vertices[idx1].m_vertex;
    const Vector4f& x2 = m_vertices[idx2].m_vertex;
    const Vector4f& x3 = m_vertices[idx3].m_vertex;

    Vector4f x12 = Vector4f((x1.x + x2.x) / 2.0f, (x1.y + x2.y) / 2.0f, (x1.z + x2.z) / 2.0f, 1.0f);
    Vector4f x23 = Vector4f((x2.x + x3.x) / 2.0f, (x2.y + x3.y) / 2.0f, (x2.z + x3.z) / 2.0f, 1.0f);
    Vector4f x31 = Vector4f((x3.x + x1.x) / 2.0f, (x3.y + x1.y) / 2.0f, (x3.z + x1.z) / 2.0f, 1.0f);

    Vector2f x12NormCoord = (m_vertices[idx1].m_normalizedMappingCoord + m_vertices[idx2].m_normalizedMappingCoord) / 2.0f;
    Vector2f x23NormCoord = (m_vertices[idx2].m_normalizedMappingCoord + m_vertices[idx3].m_normalizedMappingCoord) / 2.0f;
    Vector2f x31NormCoord = (m_vertices[idx3].m_normalizedMappingCoord + m_vertices[idx1].m_normalizedMappingCoord) / 2.0f;

    // normalize so vector has length 1
    x12.Normalize3();
    x23.Normalize3();
    x31.Normalize3();

    Vector4f radiusVec(m_radius, m_radius, m_radius, 1.0f);
    x12.Mul3(radiusVec);
    x23.Mul3(radiusVec);
    x31.Mul3(radiusVec);

    x12NormCoord = CalculateUV(x12);
    x23NormCoord = CalculateUV(x23);
    x31NormCoord = CalculateUV(x31);

    int idx12;
    int idx23;
    int idx31;

    // 12
    auto it12 = std::find(m_vertices.begin(), m_vertices.end(), OctahedronVertex{ x12, x12NormCoord });
    if (it12 == m_vertices.end())
    {
        idx12 = (int)m_vertices.size();
        m_vertices.push_back({ x12, x12NormCoord });
    }
    else
    {
        idx12 = int(it12 - m_vertices.begin());
    }

    // 23
    auto it23 = std::find(m_vertices.begin(), m_vertices.end(), OctahedronVertex{ x23, x23NormCoord });
    if (it23 == m_vertices.end())
    {
        idx23 = (int)m_vertices.size();
        m_vertices.push_back({ x23, x23NormCoord });
    }
    else
    {
        idx23 = int(it23 - m_vertices.begin());
    }

    // 31
    auto it31 = std::find(m_vertices.begin(), m_vertices.end(), OctahedronVertex{ x31, x31NormCoord });
    if (it31 == m_vertices.end())
    {
        idx31 = (int)m_vertices.size();
        m_vertices.push_back({ x31, x31NormCoord });
    }
    else
    {
        idx31 = int(it31 - m_vertices.begin());
    }

    triangulateHelper(level - 1, idx1, idx12, idx31);
    triangulateHelper(level - 1, idx12, idx2, idx23);
    triangulateHelper(level - 1, idx31, idx23, idx3);
    triangulateHelper(level - 1, idx23, idx31, idx12);
}

void Hemioctahedron::triangulateLastLevel(int idx1, int idx2, int idx3)
{
    const Vector4f& x2 = m_vertices[idx2].m_vertex;
    const Vector4f& x3 = m_vertices[idx3].m_vertex;

    Vector4f x23 = Vector4f((x2.x + x3.x) / 2.0f, (x2.y + x3.y) / 2.0f, (x2.z + x3.z) / 2.0f, 1.0f);
    Vector2f x23NormCoord = (m_vertices[idx2].m_normalizedMappingCoord + m_vertices[idx3].m_normalizedMappingCoord) / 2.0f;

    uint32_t idx23 = m_vertices.size();
    m_vertices.push_back({ x23, x23NormCoord });

    m_indices.push_back(idx1);
    m_indices.push_back(idx2);
    m_indices.push_back(idx23);

    m_indices.push_back(idx1);
    m_indices.push_back(idx23);
    m_indices.push_back(idx3);
}

const std::vector<uint32_t>& Hemioctahedron::GetIndices()
{
    return m_indices;
}