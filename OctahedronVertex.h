#pragma once

#include "Vector4f.h"
#include "Vector2.h"

using Vector2f = Vector2<float>;

struct OctahedronVertex
{
    Vector4f m_vertex;
    Vector2f m_normalizedMappingCoord;    // impostor normalized coord

    bool operator==(const OctahedronVertex& other);
    OctahedronVertex& operator*=(float v);
};

Vector2f CalculateUV(Vector4f coord);