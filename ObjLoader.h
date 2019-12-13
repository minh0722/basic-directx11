#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "MaterialLoader.h"

namespace wavefront
{
	struct Faces
	{
		std::vector<uint32_t> vertexIndices;
		uint32_t materialId;
	};

    struct VertexFormat
    {
        Vector3<float> vertex;
        Vector2<float> texcoord;
    };

    struct Obj
    {
        std::vector<Vector3<float>> vertices;
        std::vector<Vector3<float>> vertexNormals;
        std::vector<Vector2<float>> texCoord;
        std::vector<VertexFormat> vertexBuffer;
		Faces verticesFaces;
		Faces normalsFaces;
		Faces texCoordFaces;
		std::string materialFileName;

		std::map<uint32_t, Material> materials;
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);

	private:
		static void PrepareParse(std::ifstream& is, Obj& obj);
    };
}