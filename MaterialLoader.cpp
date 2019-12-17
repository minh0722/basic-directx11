#include "pch.h"
#include "MaterialLoader.h"
#include "ObjLoader.h"
#include "extern\fastcrc32\Crc32.h"

namespace wavefront
{
	void MaterialLoader::Parse(Obj& obj)
	{
		std::ifstream is(obj.materialFileName, std::ios::in | std::ios::binary);
		assert(is.good());

		while (!is.eof() && !is.fail())
		{
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

			if (StringEqual(buf, "newmtl"))
			{
				is.get();
				char buf[256];
				is.get(buf, '\r\n');
				std::string materialName = buf;
				c = is.get();

				Material& newMaterial = obj.materials[crc32_fast(materialName.c_str(), materialName.length())];
				ParseMaterialInformation(is, newMaterial);
			}
			else
			{
				IgnoreLine(is);
			}
		}

		is.close();
	}

	void MaterialLoader::ParseMaterialInformation(std::ifstream& is, Material& obj)
	{
		assert(is.good());
		bool gotAmbient = false, gotDiffuse = false, gotSpecular = false;

		while (!is.eof() && !is.fail() && (!gotAmbient || !gotDiffuse || !gotSpecular))
		{
			char buf[20];

			is.get(buf, 20, ' ');

			if (StringEqual(buf, "Ka"))
			{
				Vector4f& ambient = obj.ambient;
				is >> ambient[0] >> ambient[1] >> ambient[2];
				IgnoreLine(is);
				gotAmbient = true;
			}
			else if (StringEqual(buf, "Kd"))
			{
				Vector4f& diffuse = obj.diffuse;
				is >> diffuse[0] >> diffuse[1] >> diffuse[2];
				IgnoreLine(is);
				gotDiffuse = true;
			}
			else if (StringEqual(buf, "Ks"))
			{
				Vector4f& specular = obj.specular;
				is >> specular[0] >> specular[1] >> specular[2];
				IgnoreLine(is);
				gotSpecular = true;
			}
		}
	}
}