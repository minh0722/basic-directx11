#pragma once
#include "pch.h"

class Matrix44f;

template<typename T>
class Vector3;

class Vector4f
{
public:
    Vector4f() {}
	Vector4f(const Vector3<float>& v, float w);
	Vector4f(float x, float y, float z, float w);
	Vector4f(const Vector4f& other);
    Vector4f(const DirectX::XMVECTOR& other);
	Vector4f& operator=(const Vector4f& other);
    Vector4f operator-(const Vector4f& other) const;
    Vector4f operator/(float num) const;
    Vector4f operator*(const Matrix44f& other) const;
    Vector4f& operator*=(const float num);
    Vector4f& operator+=(const float num);
    bool operator==(const Vector4f& other) const;
    Vector4f CrossProduct(const Vector4f& other) const;
	float DotProduct(const Vector4f& other) const;
    float DotProduct3(const Vector4f& other) const;

    float& operator[](uint16_t index);
    float operator[](uint16_t index) const;

    float GetLength() const;

    void Normalize3();

    void Mul3(const Vector4f& other);
    Vector4f Mul3(const Vector4f& other) const;

    Vector4f Abs() const;

public:
    union
    {
		DirectX::XMVECTOR m_v;
        float m_fValues[4];
        struct  
        {
            float x, y, z, w;
        };
        struct  
        {
            float r, g, b, a;
        };
    };


};