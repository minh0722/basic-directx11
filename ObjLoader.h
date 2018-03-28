#pragma once
#include <fstream>
#include <vector>

namespace wavefront
{
    struct Obj
    {
        std::vector<std::tuple<float, float, float>> vertices;
        std::vector<std::tuple<float, float, float>> vertexNormals;
        std::vector<std::tuple<float, float>> texCoord;
		std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> vertexIndices;
		std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> vertexNormalIndices;
		std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> texCoordIndices;
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);
    };
}