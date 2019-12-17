#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "MaterialLoader.h"

namespace wavefront
{
    struct AABB
    {
        Vector4f center;
        Vector4f halfVec;
    };

    struct VertexFormat
    {
        Vector3<float> vertex;
        Vector2<float> texcoord;
    };

	struct Faces
	{
        std::vector<VertexFormat> vertexBuffer;
	};

    enum DrawType
    {
        Draw,                   // non-indexed, non-instanced
        DrawIndexed,            // indexed, non-instanced
        DrawIndexedInstanced,   // indexed, instanced
        DrawInstanced,          // non-indexed, instanced
        DrawTypeCount
    };

    struct Obj
    {
        std::vector<Vector3<float>> vertices;
        std::vector<Vector3<float>> vertexNormals;
        std::vector<Vector2<float>> texCoord;
		std::string materialFileName;
        DrawType drawType;
        AABB boundingBox;

        std::map<uint32_t, Faces> perMaterialFaces;
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