#include "pch.h"
#include "Octahedron.h"

Octahedron::Octahedron(float radius)
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
