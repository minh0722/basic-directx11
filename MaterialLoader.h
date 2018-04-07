#pragma once

#include "Vector3.h"
#include <map>

namespace wavefront
{
	struct Material
	{
		Vector3<float> ambient;
		Vector3<float> diffuse;
		Vector3<float> specular;
		size_t materialID;
	};

	class MaterialLoader final
	{
	public:
		static Material Parse(const char* file);

	private:
		static std::map<std::string, size_t> gs_MaterialIndexer;
	};
}