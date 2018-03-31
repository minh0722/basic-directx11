#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"

namespace wavefront
{
    struct Obj
    {
        std::vector<Vector3<float>> vertices;
        std::vector<Vector3<float>> vertexNormals;
        std::vector<Vector2<float>> texCoord;
		std::vector<Vector3<uint32_t>> vertexIndices;
		std::vector<Vector3<uint32_t>> vertexNormalIndices;
		std::vector<Vector3<uint32_t>> texCoordIndices;
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);
    };
}