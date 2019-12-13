#include "pch.h"
#include "OctahedronVertex.h"

Vector4f sign(Vector4f v)
{
    return Vector4f{
        v.x < 0.0f ? -1.0f : (v.x == 0 ? 0.0f : 1.0f),
        v.y < 0.0f ? -1.0f : (v.y == 0 ? 0.0f : 1.0f),
        v.z < 0.0f ? -1.0f : (v.z == 0 ? 0.0f : 1.0f),
        0.0f
    };
}

Vector2f CalculateUV(Vector4f coord)
{
    float x = coord.x;
    float y = coord.y;
    float z = coord.z;

    //Vector2f res{
    //    float(0.5 * (x / (1.0f + y) + 1.0f)),
    //    abs(float(0.5 * (z / (1.0f + y) + 1.0f)) - 1.0f) };

    Vector4f octant = sign(coord);
    float sum = coord.DotProduct3(octant);
    Vector4f octahedron = coord / sum;
    
    if (octahedron.y < 0)
    {
        Vector4f absolute = octahedron.Abs();
        octahedron.x = octant.x * (1.0f - absolute.z);
        octahedron.z = octant.z * (1.0f - absolute.x);
    }

    //return res;
    return { float(octahedron.x * 0.5f + 0.5f), abs(float(octahedron.z * 0.5f + 0.5) - 1.0f)};
}

bool OctahedronVertex::operator==(const OctahedronVertex& other)
{
    return m_vertex == other.m_vertex;
}

OctahedronVertex& OctahedronVertex::operator*=(float v)
{
    m_vertex *= v;
    return *this;
}