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

static const UINT screenWidth = 1080;
static const UINT screenHeight = 720;

#define THROW_IF_FAILED(hr) SafetyCheck::ThrowIfFailed(hr, __FILE__, __LINE__)
#define THROW_IF_NULL(ptr) SafetyCheck::ThrowIfNull(ptr, __FILE__, __LINE__)
#define THROW_IF_FALSE(flag) SafetyCheck::ThrowIfFalse(flag, __FILE__, __LINE__)

struct Position
{
	float x, y, z, w;
};

struct Color
{
	float r, g, b, a;
};

struct Vertex
{
	Position pos;
	Color color;

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layouts(2);
		layouts[0].SemanticName = "POSITION";
		layouts[0].SemanticIndex = 0;								// will use POSITION0 semantic
		layouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
		layouts[0].InputSlot = 0;									// 0 ~ 15
		layouts[0].AlignedByteOffset = 0;
		layouts[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
		layouts[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

		layouts[1].SemanticName = "COLOR";
		layouts[1].SemanticIndex = 0;
		layouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layouts[1].InputSlot = 0;
		layouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layouts[1].InstanceDataStepRate = 0;

		return layouts;
	}
};


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

