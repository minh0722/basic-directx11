#include "ObjLoader.h"
#include <cassert>
#include <limits>	// numeric_limits
#include <cstring>

namespace wavefront
{
    bool StringEqual(const char* c1, const char* c2)
    {
        return strcmp(c1, c2) == 0;
    }

    bool CharEqual(const char c1, const char c2)
    {
        return c1 == c2;
    }

    std::istream& IgnoreLine(std::ifstream& is)
    {
        return is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

	std::istream& IgnoreUntilSlash(std::ifstream& is)
	{
		return is.ignore(std::numeric_limits<std::streamsize>::max(), '/');
	}

    std::string GetFileDirectory(const char* file)
    {
        std::string fileStr(file);

        size_t lastDash = fileStr.find_last_of('/');
        fileStr.erase(fileStr.begin() + lastDash, fileStr.end());
        fileStr.append("/");

        return fileStr;
    }

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
				is >> vertexIdx1; 
				IgnoreUntilSlash(is); is >> vertexNormalIdx1;
				IgnoreUntilSlash(is); is >> texCoordIdx1 >> std::ws;
				//is.get();
				is >> vertexIdx2;
				IgnoreUntilSlash(is); is >> vertexNormalIdx2;
				IgnoreUntilSlash(is); is >> texCoordIdx2 >> std::ws;
				//is.get();
				is >> vertexIdx3;
				IgnoreUntilSlash(is); is >> vertexNormalIdx3;
				IgnoreUntilSlash(is); is >> texCoordIdx3;
				IgnoreLine(is);

				result.vertexIndices.push_back(Vector3<uint32_t>(vertexIdx1 - 1, vertexIdx2 - 1, vertexIdx3 - 1));
				result.vertexNormalIndices.push_back(Vector3<uint32_t>(vertexNormalIdx1 - 1, vertexNormalIdx2 - 1, vertexNormalIdx3 - 1));
				result.texCoordIndices.push_back(Vector3<uint32_t>(texCoordIdx1 - 1, texCoordIdx2 - 1, texCoordIdx3 - 1));
			}
			else if (StringEqual(buf, "usemtl") || StringEqual(buf, "usemap"))
			{
				IgnoreLine(is);
			}
            else if (StringEqual(buf, "mtllib"))
            {
                std::string str = GetFileDirectory(file);
                is.get();
                is.get(buf, 256, ' ');
                str.append(buf);

                IgnoreLine(is);
            }
			else
			{
				IgnoreLine(is);
			}
        }

        char buf[256];
        strerror_s(buf, 256, errno);

        return result;
    }
}