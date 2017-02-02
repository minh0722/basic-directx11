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
#include <cassert>

using namespace Microsoft::WRL;

static const UINT screenWidth = 1080;
static const UINT screenHeight = 720;

#define THROW_IF_FAILED(hr) SafetyCheck::ThrowIfFailed(hr, __FILE__, __LINE__)
#define THROW_IF_NULL(ptr) SafetyCheck::ThrowIfNull(ptr, __FILE__, __LINE__)
#define THROW_IF_FALSE(flag) SafetyCheck::ThrowIfFalse(flag, __FILE__, __LINE__)

#define PI 3.14159265359
#define RADIAN 0.01745329252

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
	
	float operator*(const Vector4f& other)
	{
		return nums[0] * other.nums[0] +
			nums[1] * other.nums[1] +
			nums[2] * other.nums[2] +
			nums[3] * other.nums[3];
	}

	void SetColumn(size_t index, float newValue)
	{
		assert(index < 4);
		nums[index] = newValue;
	}
    
	float nums[4];
};

struct Matrix44f
{
    Vector4f m_v[4];

    Matrix44f(float v00, float v01, float v02, float v03,
              float v10, float v11, float v12, float v13,
              float v20, float v21, float v22, float v23,
              float v30, float v31, float v32, float v33)
    {
        m_v[0] = { v00, v01, v02, v03 };
        m_v[1] = { v10, v11, v12, v13 };
        m_v[2] = { v20, v21, v22, v23 };
        m_v[3] = { v30, v31, v32, v33 };
    }

	Matrix44f(const Matrix44f& other)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_v[i] = other.m_v[i];
		}
	}

    Matrix44f()
    {
        m_v[0].nums[0] = 1.0f;
        m_v[1].nums[1] = 1.0f;
        m_v[2].nums[2] = 1.0f;
        m_v[3].nums[3] = 1.0f;
    }

	Matrix44f Transpose() const
	{
		return { m_v[0].nums[0], m_v[1].nums[0], m_v[2].nums[0], m_v[3].nums[0],
				 m_v[0].nums[1], m_v[1].nums[1], m_v[2].nums[1], m_v[3].nums[1],
				 m_v[0].nums[2], m_v[1].nums[2], m_v[2].nums[2], m_v[3].nums[2],
				 m_v[0].nums[3], m_v[1].nums[3], m_v[2].nums[3], m_v[3].nums[3] };
	}

	void SetColumn(size_t index, const Vector4f& column)
	{
		for (size_t i = 0; i < 4; ++i)
		{
			m_v[i].nums[index] = column.nums[i];
		}
	}

	Matrix44f& operator=(const Matrix44f& other)
	{
		if (this != &other)
		{
			for (size_t i = 0; i < 4; ++i)
			{
				m_v[i] = other.m_v[i];
			}
		}

		return *this;
	}

	Matrix44f operator*(const Matrix44f& other)
	{
		Matrix44f transposed = other.Transpose();

		return { m_v[0] * transposed.m_v[0], m_v[0] * transposed.m_v[1], m_v[0] * transposed.m_v[2], m_v[0] * transposed.m_v[3],
				 m_v[1] * transposed.m_v[0], m_v[1] * transposed.m_v[1], m_v[1] * transposed.m_v[2], m_v[1] * transposed.m_v[3],
				 m_v[2] * transposed.m_v[0], m_v[2] * transposed.m_v[1], m_v[2] * transposed.m_v[2], m_v[2] * transposed.m_v[3],
				 m_v[3] * transposed.m_v[0], m_v[3] * transposed.m_v[1], m_v[3] * transposed.m_v[2], m_v[3] * transposed.m_v[3] };
	}

	Matrix44f& operator*=(const Matrix44f& other)
	{
		Matrix44f result = this->operator*(other);
		*this = result;

		return *this;
	}

    Vector4f operator*(const Vector4f& other)
    {
        Vector4f result;

        result = { m_v[0] * other, m_v[1] * other, m_v[2] * other, m_v[3] * other };

        return result;
    }

	float GetDeterminant()
	{
		float a11 = m_v[0].nums[0], a12 = m_v[0].nums[0], a13 = m_v[0].nums[0], a14 = m_v[0].nums[0];
		float a21 = m_v[0].nums[0], a22 = m_v[0].nums[0], a23 = m_v[0].nums[0], a24 = m_v[0].nums[0];
		float a31 = m_v[0].nums[0], a32 = m_v[0].nums[0], a33 = m_v[0].nums[0], a34 = m_v[0].nums[0];
		float a41 = m_v[0].nums[0], a42 = m_v[0].nums[0], a43 = m_v[0].nums[0], a44 = m_v[0].nums[0];

		return a11*a22*a33*a44 + a11*a23*a34*a42 + a11*a24*a32*a43 +
			a12*a21*a34*a43 + a12*a23*a31*a44 + a12*a24*a33*a41 +
			a13*a21*a32*a44 + a13*a22*a34*a41 + a13*a24*a31*a42 +
			a14*a21*a33*a42 + a14*a22*a31*a43 + a14*a23*a32*a41 -
			a11*a22*a34*a43 - a11*a23*a32*a44 - a11*a24*a33*a42 -
			a12*a21*a33*a44 - a12*a23*a34*a41 - a12*a24*a31*a43 -
			a13*a21*a34*a42 - a13*a22*a31*a44 - a13*a24*a32*a41 -
			a14*a21*a32*a43 - a14*a22*a33*a41 - a14*a23*a31*a42;
	}

	Matrix44f Inverse()
	{
		float determinant = GetDeterminant();

		float a11 = m_v[0].nums[0], a12 = m_v[0].nums[0], a13 = m_v[0].nums[0], a14 = m_v[0].nums[0];
		float a21 = m_v[0].nums[0], a22 = m_v[0].nums[0], a23 = m_v[0].nums[0], a24 = m_v[0].nums[0];
		float a31 = m_v[0].nums[0], a32 = m_v[0].nums[0], a33 = m_v[0].nums[0], a34 = m_v[0].nums[0];
		float a41 = m_v[0].nums[0], a42 = m_v[0].nums[0], a43 = m_v[0].nums[0], a44 = m_v[0].nums[0];

		float b11 = a22*a33*a44 + a23*a34*a42 + a24*a32*a43 - a22*a34*a43 - a23*a32*a44 - a24*a33*a42;
		float b12 = a12*a34*a43 + a13*a32*a44 + a14*a33*a42 - a12*a33*a44 - a13*a34*a42 - a14*a32*a43;
		float b13 = a12*a23*a44 + a13*a24*a42 + a14*a22*a43 - a12*a24*a43 - a13*a22*a44 - a14*a23*a42;
		float b14 = a12*a24*a33 + a13*a22*a34 + a14*a23*a32 - a12*a23*a34 - a13*a24*a32 - a14*a22*a33;
		//float b21 = a21*a34*a43 + 
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

