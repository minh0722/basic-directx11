#pragma once
#include <vector>
#include "Vector2.h"
#include "Vector3.h"
#include "MaterialLoader.h"

namespace wavefront
{
    struct AABB
    {
        AABB() {}
		
        void Set(const Vector3<float>& min, const Vector3<float>& max)
		{
			Vector3<float> c = (max + min) / 2.0f;
            Vector3<float> h = (max - min) / 2.0f;

			m_center = Vector4f(c.x, c.y, c.z, 1.0f);
            m_halfVec = Vector4f(h.x, h.y, h.z, 1.0f);
		}

        float GetRadius() const
        {
            return m_halfVec.GetLength();
        }

        Vector4f m_center = {};
        Vector4f m_halfVec = {};
    };

    struct VertexFormat
    {
        Vector3<float> vertex;
        Vector3<float> normal;
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

        D3D11_PRIMITIVE_TOPOLOGY m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
        std::vector<D3D11_INPUT_ELEMENT_DESC> vertexShaderInputLayout = 
        {
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},                             // vertex
            {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},    // normal
            {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}      // uv coord
        };
    };

    class ObjLoader final
    {
    public:
        static Obj Parse(const char* file);

	private:
		static void PrepareParse(std::ifstream& is, Obj& obj);
    };
}