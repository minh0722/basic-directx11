#include "ObjLoader.h"
#include <cassert>
#include <tuple>
#include <limits>	// numeric_limits

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

    void IgnoreLine(std::ifstream& is)
    {
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    Obj ObjLoader::Parse(const char* file)
    {
        std::ifstream is(file, std::ios::in | std::ios::binary);
        assert(is.good());
        
        float x, y, z;
        std::streampos pos;
        Obj result;

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
                result.vertices.push_back(std::make_tuple(x, y, z));
                continue;
            }
            else if (StringEqual(buf, "vn"))
            {
                is.get();
                is >> x >> y >> z;
                result.vertexNormals.push_back(std::make_tuple(x, y, z));
                continue;
            }
            else if (StringEqual(buf, "vt"))
            {
                is.get();
                is >> x >> y;
                IgnoreLine(is);     // ignore z if there is z

                result.texCoord.push_back(std::make_tuple(x, y));
            }
            else if (StringEqual(buf, "mtllib"))
            {
                IgnoreLine(is);
            }
        }

        char buf[256];
        strerror_s(buf, 256, errno);

        return result;
    }
}