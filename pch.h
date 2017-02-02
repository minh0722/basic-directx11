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

using namespace Microsoft::WRL;

static const UINT screenWidth = 1080;
static const UINT screenHeight = 720;

#define THROW_IF_FAILED(hr) SafetyCheck::ThrowIfFailed(hr, __FILE__, __LINE__)
#define THROW_IF_NULL(ptr) SafetyCheck::ThrowIfNull(ptr, __FILE__, __LINE__)
#define THROW_IF_FALSE(flag) SafetyCheck::ThrowIfFalse(flag, __FILE__, __LINE__)

enum Axis
{
    X,
    Y,
    Z,
    W,
    AxisCount
};

struct Vector4f
{
    Vector4f() {}

    Vector4f& operator=(const Vector4f& other)
    {
        if (this != &other)
        {
            nums[0] = other.nums[0];
            nums[1] = other.nums[1];
            nums[2] = other.nums[2];
            nums[3] = other.nums[3];
        }

        return *this;
    }

    Vector4f(const Vector4f& other)
    {
        nums[0] = other.nums[0];
        nums[1] = other.nums[1];
        nums[2] = other.nums[2];
        nums[3] = other.nums[3];
    }

    Vector4f(float n0, float n1, float n2, float n3) 
    { 
        nums[0] = n0;
        nums[1] = n1; 
        nums[2] = n2;
        nums[3] = n3;
    }

    float nums[4];

	float operator*(const Vector4f& other)
	{
		return nums[0] * other.nums[0] +
			nums[1] * other.nums[1] +
			nums[2] * other.nums[2] +
			nums[3] * other.nums[3];
	}
};

struct Matrix44f
{
    Vector4f v[4];

    Matrix44f(float v00, float v01, float v02, float v03,
              float v10, float v11, float v12, float v13,
              float v20, float v21, float v22, float v23,
              float v30, float v31, float v32, float v33)
    {
        v[0] = { v00, v01, v02, v03 };
        v[1] = { v10, v11, v12, v13 };
        v[2] = { v20, v21, v22, v23 };
        v[3] = { v30, v31, v32, v33 };
    }

    Matrix44f()
    {
        v[0].nums[0] = 1.0f;
        v[1].nums[1] = 1.0f;
        v[2].nums[2] = 1.0f;
        v[3].nums[3] = 1.0f;
    }

	Matrix44f Transpose() const
	{
		return { v[0].nums[0], v[1].nums[0], v[2].nums[0], v[3].nums[0],
				 v[0].nums[1], v[1].nums[1], v[2].nums[1], v[3].nums[1],
				 v[0].nums[2], v[1].nums[2], v[2].nums[2], v[3].nums[2],
				 v[0].nums[3], v[1].nums[3], v[2].nums[3], v[3].nums[3] };
	}

	Matrix44f operator*(const Matrix44f& other)
	{
		Matrix44f transposed = other.Transpose();

		return { v[0] * transposed.v[0], v[0] * transposed.v[1], v[0] * transposed.v[2], v[0] * transposed.v[3],
				 v[1] * transposed.v[0], v[1] * transposed.v[1], v[1] * transposed.v[2], v[1] * transposed.v[3],
				 v[2] * transposed.v[0], v[2] * transposed.v[1], v[2] * transposed.v[2], v[2] * transposed.v[3],
				 v[3] * transposed.v[0], v[3] * transposed.v[1], v[3] * transposed.v[2], v[3] * transposed.v[3] };
	}

    Vector4f operator*(const Vector4f& other)
    {
        Vector4f result;

        result = { v[0] * other, v[1] * other, v[2] * other, v[3] * other };

        return result;
    }
};

struct Color
{
	float r, g, b, a;
};

struct Vertex
{
	Vector4f pos;
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

