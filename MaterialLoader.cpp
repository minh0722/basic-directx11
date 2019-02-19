#include "pch.h"
#include "MaterialLoader.h"
#include "ObjLoader.h"
#include <atomic>

namespace wavefront
{
	void MaterialLoader::Parse(const char* file, Obj& obj)
	{
		std::ifstream is(file, std::ios::in | std::ios::binary);
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
				std::string materialName;
				is.get(materialName.data(), '\r\n');
				c = is.get();

				Material& newMaterial = obj.materials[materialName.c_str()];
				ParseMaterialInformation(is, newMaterial);
			}
			else
			{
				IgnoreLine(is);
			}
		}
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
				Vector3<float>& ambient = obj.ambient;
				is >> ambient[0] >> ambient[1] >> ambient[2];
				IgnoreLine(is);
				gotAmbient = true;
			}
			else if (StringEqual(buf, "Kd"))
			{
				Vector3<float>& diffuse = obj.diffuse;
				is >> diffuse[0] >> diffuse[1] >> diffuse[2];
				IgnoreLine(is);
				gotDiffuse = true;
			}
			else if (StringEqual(buf, "Ks"))
			{
				Vector3<float>& specular = obj.specular;
				is >> specular[0] >> specular[1] >> specular[2];
				IgnoreLine(is);
				gotSpecular = true;
			}
		}
	}
}