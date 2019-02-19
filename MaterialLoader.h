#pragma once

#include "Vector3.h"
#include <map>

namespace wavefront
{
	struct Obj;

	struct Material
	{
		Vector3<float> ambient;
		Vector3<float> diffuse;
		Vector3<float> specular;
	};

	class MaterialLoader final
	{
	public:
		static void Parse(Obj& obj);

	private:
		static void ParseMaterialInformation(std::ifstream& is, Material& obj);
	};
}