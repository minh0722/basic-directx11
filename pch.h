//
// pch.h
// Header for standard system include files.
//

#pragma once

// Use the C++ standard templated min/max
#define NOMINMAX

#include <wrl.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXColors.h>

#include <algorithm>
#include <memory>

#include <iostream>
#include <immintrin.h>
#include <vector>
#include <chrono>
#include <ppl.h>
#include <random>
#include <cmath>
#include <fstream>
#include <cassert>
#include <limits>	// numeric_limits
#include <cstring>

#include "common.h"

static const UINT screenWidth = 1080;
static const UINT screenHeight = 720;

#define THROW_IF_FAILED(hr) SafetyCheck::ThrowIfFailed(hr, __FILE__, __LINE__)
#define THROW_IF_NULL(ptr) SafetyCheck::ThrowIfNull(ptr, __FILE__, __LINE__)
#define THROW_IF_FALSE(flag) SafetyCheck::ThrowIfFalse(flag, __FILE__, __LINE__)

static const float PI = 3.14159265359f;
static const float RADIAN = 0.01745329252f;

#if 0
#define OUTPUT_DEBUG(format, ...) \
	char buf[256];	\
	snprintf(buf, 256, format, __VA_ARGS__); \
	OutputDebugStringA(buf);
#else
#define OUTPUT_DEBUG(format, ...)
#endif

enum RotationAxis
{
    Roll,
	Pitch,
	Yaw,
    AxisCount
};

struct WorldViewProj
{
    DirectX::XMMATRIX world;
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX proj;
};

namespace SafetyCheck
{
	inline void ThrowIfFailed(HRESULT hr, const char* file, int line)
	{
		if (FAILED(hr))
		{
			std::cerr << "DirectX exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}

	inline void ThrowIfNull(void* ptr, const char* file, int line)
	{
		if (!ptr)
		{
			std::cerr << "Null pointer exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}

	inline void ThrowIfFalse(bool flag, const char* file, int line)
	{
		if (!flag)
		{
			std::cerr << "Bool exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}
}


namespace wavefront
{
	inline bool StringEqual(const char* c1, const char* c2)
	{
		return strcmp(c1, c2) == 0;
	}

	inline bool CharEqual(const char c1, const char c2)
	{
		return c1 == c2;
	}

	inline std::istream& IgnoreLine(std::ifstream& is)
	{
		return is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	}

	inline std::istream& IgnoreUntilSlash(std::ifstream& is)
	{
		return is.ignore(std::numeric_limits<std::streamsize>::max(), '/');
	}

	inline std::string GetFileDirectory(const char* file)
	{
		std::string fileStr(file);

		size_t lastDash = fileStr.find_last_of('/');
		fileStr.erase(fileStr.begin() + lastDash, fileStr.end());
		fileStr.append("/");

		return fileStr;
	}
}
