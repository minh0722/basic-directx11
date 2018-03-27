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
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);
    };
}