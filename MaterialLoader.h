#pragma once

#include "Vector3.h"
#include "Vector4f.h"
#include <map>

namespace wavefront
{
	struct Obj;

	struct Material
	{
        Vector4f ambient = {};
        Vector4f diffuse = {};
        Vector4f specular = {};
	};

	class MaterialLoader final
	{
	public:
		static void Parse(Obj& obj);

	private:
		static void ParseMaterialInformation(std::ifstream& is, Material& obj);
	};
}