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

using namespace Microsoft::WRL;

#define THROW_IF_FAILED(hr) SafetyCheck::ThrowIfFailed(hr, __FILE__, __LINE__)
#define THROW_IF_NULL(ptr) SafetyCheck::ThrowIfNull(ptr, __FILE__, __LINE__)
#define THROW_IF_FALSE(flag) SafetyCheck::ThrowIfFalse(flag, __FILE__, __LINE__)

namespace SafetyCheck
{
	inline void ThrowIfFailed(HRESULT hr, char* file, int line)
	{
		if (FAILED(hr))
		{
			std::cerr << "DirectX exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}

	inline void ThrowIfNull(void* ptr, char* file, int line)
	{
		if (!ptr)
		{
			std::cerr << "Null pointer exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}

	inline void ThrowIfFalse(bool flag, char* file, int line)
	{
		if (!flag)
		{
			std::cerr << "Bool exception in file " << file << " on line " << line << std::endl;
			throw std::exception();
		}
	}
}

