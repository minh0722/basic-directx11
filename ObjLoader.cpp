#include "pch.h"
#include "ObjLoader.h"
#include "MaterialLoader.h"
#include "extern/fastcrc32/Crc32.h"

#include <limits>
#include <cassert>

namespace wavefront
{
    Obj ObjLoader::Parse(const char* file)
    {
        std::ifstream is(file, std::ios::in | std::ios::binary);
        assert(is.good());

        float x, y, z;
        std::streampos pos;
        Obj result;
		uint32_t vertexIdx1, vertexIdx2, vertexIdx3;
		uint32_t vertexNormalIdx1, vertexNormalIdx2, vertexNormalIdx3;
		uint32_t texCoordIdx1, texCoordIdx2, texCoordIdx3;

		pos = is.tellg();
		PrepareParse(is, result);
		is.seekg(0, is.beg);

		MaterialLoader::Parse(result);

		uint32_t currentFaceMaterialCrc = 0;

        // find minimum bounding box
		const float MAX_FLOAT = std::numeric_limits<float>::max();
		const float MIN_FLOAT = std::numeric_limits<float>::min();
		Vector3<float> min(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
		Vector3<float> max(MIN_FLOAT, MIN_FLOAT, MIN_FLOAT);

        while (!is.eof() && !is.fail())
        {
            pos = is.tellg();

            char buf[256];
            char c = is.peek();

            if (CharEqual(c, '#') || CharEqual(c, '\n') || CharEqual(c, '\r'))
            {
                IgnoreLine(is);
                continue;
            }
            else if (CharEqual(c, ' '))
            {
                is.get();
                continue;
            }

            is.get(buf, 256, ' ');

            if (StringEqual(buf, "g"))
            {
                IgnoreLine(is);
                continue;
            }
            else if (StringEqual(buf, "v"))
            {
                is.get();
                is >> x >> y >> z;
                result.vertices.push_back(Vector3<float>(x, y, z));

                min.x = std::min(min.x, x); max.x = std::max(max.x, x);
                min.y = std::min(min.y, y); max.y = std::max(max.y, y);
                min.z = std::min(min.z, z); max.z = std::max(max.z, z);

                continue;
            }
            else if (StringEqual(buf, "vn"))
            {
                is.get();
                is >> x >> y >> z;
                result.vertexNormals.push_back(Vector3<float>(x, y, z));
                continue;
            }
            else if (StringEqual(buf, "vt"))
            {
                is.get();
                is >> x >> y;
                IgnoreLine(is);     // ignore z if there is z

                result.texCoord.push_back(Vector2<float>(x, y));
            }
			else if (StringEqual(buf, "f"))
			{
				is.get();
				is >> vertexIdx1;					IgnoreUntilSlash(is); 
				is >> texCoordIdx1;					IgnoreUntilSlash(is);
				is >> vertexNormalIdx1 >> std::ws;
				//is.get();
				is >> vertexIdx2;					IgnoreUntilSlash(is); 
				is >> texCoordIdx2;					IgnoreUntilSlash(is);
				is >> vertexNormalIdx2 >> std::ws;
				//is.get();
				is >> vertexIdx3;				IgnoreUntilSlash(is); 
				is >> texCoordIdx3;				IgnoreUntilSlash(is);
				is >> vertexNormalIdx3;
				IgnoreLine(is);

                --vertexIdx1;  --texCoordIdx1; --vertexNormalIdx1;
                --vertexIdx2;  --texCoordIdx2; --vertexNormalIdx2;
                --vertexIdx3;  --texCoordIdx3; --vertexNormalIdx3;

                result.perMaterialFaces[currentFaceMaterialCrc].vertexBuffer.push_back(VertexFormat{ result.vertices[vertexIdx1], result.vertexNormals[vertexNormalIdx1], result.texCoord[texCoordIdx1] });
                result.perMaterialFaces[currentFaceMaterialCrc].vertexBuffer.push_back(VertexFormat{ result.vertices[vertexIdx2], result.vertexNormals[vertexNormalIdx2], result.texCoord[texCoordIdx2] });
                result.perMaterialFaces[currentFaceMaterialCrc].vertexBuffer.push_back(VertexFormat{ result.vertices[vertexIdx3], result.vertexNormals[vertexNormalIdx3], result.texCoord[texCoordIdx3] });
			}
			else if (StringEqual(buf, "usemtl"))
			{
				is.get();
				is.get(buf, 256, '\n');
				currentFaceMaterialCrc = crc32_fast(buf, strlen(buf));
                result.perMaterialFaces[currentFaceMaterialCrc];
			}
			else
			{
				IgnoreLine(is);
			}
        }

        result.boundingBox.Set(min, max);

        if (result.vertices.size() == result.texCoord.size())
        {
            result.drawType = DrawType::DrawIndexed;
        }
        else
        {
            result.drawType = DrawType::Draw;
        }

        return result;
    }

	void ObjLoader::PrepareParse(std::ifstream& is, Obj& obj)
	{
		assert(is.good());

		std::streampos pos = is.tellg();
		int vertexPosCount = 0;
		int vertexNormalCount = 0;
		int uvCoordCount = 0;
		int faceCount = 0;

		while (!is.eof() && !is.fail())
		{
			pos = is.tellg();

			char buf[256];
			char c = is.peek();

			if (CharEqual(c, '#') || CharEqual(c, '\n') || CharEqual(c, '\r'))
			{
				IgnoreLine(is);
				continue;
			}
			else if (CharEqual(c, ' '))
			{
				is.get();
				continue;
			}

			is.get(buf, 256, ' ');

			if (StringEqual(buf, "v"))
			{
				++vertexPosCount;
				IgnoreLine(is);
				continue;
			}
			else if (StringEqual(buf, "vn"))
			{
				++vertexNormalCount;
				is.get();
				continue;
			}
			else if (StringEqual(buf, "vt"))
			{
				++uvCoordCount;
				is.get();
			}
			else if (StringEqual(buf, "f"))
			{
				++faceCount;
				is.get();
			}
			else if (StringEqual(buf, "usemtl") || StringEqual(buf, "usemap"))
			{
				IgnoreLine(is);
			}
			else if (StringEqual(buf, "mtllib"))
			{
				is.get();
				is.get(buf, 256, ' '); // TODO: need to find a way to read until \n\r too
				obj.materialFileName = "../../../assets/models/";
				obj.materialFileName += buf;
			}
			else
			{
				IgnoreLine(is);
			}
		}

		obj.vertices.reserve(vertexPosCount);
		obj.texCoord.reserve(uvCoordCount);
		obj.vertexNormals.reserve(vertexNormalCount);

		// clear eof bit so we can work with the input stream again
		is.clear();
	}
}