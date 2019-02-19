#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "MaterialLoader.h"

namespace wavefront
{
	struct Faces
	{
		std::vector<Vector3<uint32_t>> vertexIndices;
		size_t materialId;
	};

    struct Obj
    {
        std::vector<Vector3<float>> vertices;
        std::vector<Vector3<float>> vertexNormals;
        std::vector<Vector2<float>> texCoord;
		Faces verticesFaces;
		Faces normalsFaces;
		Faces texCoordFaces;
		std::string materialFileName;

		// TODO: dont map by const char* as key. Use some crc instead
		std::map<const char*, Material> materials;
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);

	private:
		static void PrepareParse(std::ifstream& is, Obj& obj);
    };
}