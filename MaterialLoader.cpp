#include "pch.h"
#include "MaterialLoader.h"
#include <atomic>

namespace wavefront
{
	std::map<std::string, size_t, MaterialLoader::StringComparer> MaterialLoader::gs_MaterialIndexer = {};

	Material MaterialLoader::Parse(const char* file)
	{
		std::ifstream is(file, std::ios::in | std::ios::binary);
		assert(is.good());

		Material result;

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

				static std::atomic<size_t> s_id = 0;
				gs_MaterialIndexer[materialName] = s_id++;
			}
			else
			{
				IgnoreLine(is);
			}
		}

		return result;
	}
}