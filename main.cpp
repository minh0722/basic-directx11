#include "pch.h"
#include <iostream>

#include "renderer.h"
#include "systemclass.h"
#include <fstream>
#include <limits>	// numeric_limits

#pragma optimize("", off)

bool StringEqual(const char* c1, const char* c2)
{
	return strcmp(c1, c2) == 0;
}

bool CharEqual(const char c1, const char* c2)
{
	return c1 == c2[0];
}

bool CharEqual(const char c1, const char c2)
{
	return c1 == c2;
}

void IgnoreLine(std::ifstream& is)
{
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

auto get(std::istream is, char* buf, char delim)
{
	auto ret = is.get();

}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	std::ifstream is("../../../assets/Models/spaceCraft6.obj", std::ios::in | std::ios::binary);

	std::vector<std::tuple<float, float, float>> v;
	
	float x, y, z;
	std::streampos pos;

	while (!is.eof() && !is.fail())
	{
		pos = is.tellg();

		char buf[256];
		char c = is.peek();

		if (CharEqual(c, '#') || CharEqual(c, '\n'))
		{
			IgnoreLine(is);
			continue;
		}
		else if (CharEqual(c, ' '))
		{
			is.get();
			continue;
		}
		else if (CharEqual(c, 'g'))
		{
			IgnoreLine(is);
			continue;
		}
		else if (CharEqual(c, 'v'))
		{
			is.get();
			is >> x >> y >> z;
			continue;
		}

		is.get(buf, 256, ' ');

		if (StringEqual(buf, "mtllib"))
		{
			IgnoreLine(is);
		}
	}

	char buf[256];
	strerror_s(buf, 256, errno);


	SystemClass* system = new SystemClass;
	system->Run();
	delete system;


	return 0;
}