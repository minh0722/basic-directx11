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

		struct StringComparer
		{
			bool operator()(const std::string& l, const std::string& r) const
			{
				return strcmp(l.c_str(), r.c_str()) == 0;
			}
		};

		static std::map<std::string, size_t, StringComparer> gs_MaterialIndexer;
	};
}