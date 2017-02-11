#pragma once
#include "pch.h"

class Vector4f
{
public:
    Vector4f() {}
	Vector4f(float x, float y, float z, float w);
	Vector4f(const Vector4f& other);
    Vector4f(const XMVECTOR& other);
	Vector4f& operator=(const Vector4f& other);
    Vector4f operator-(const Vector4f& other) const;
    Vector4f operator/(float num) const;
    Vector4f CrossProduct(const Vector4f& other) const;
	float DotProduct(const Vector4f& other) const;

    float& operator[](uint16_t index);
    float operator[](uint16_t index) const;


    float GetLength() const;
private:
    union
    {
        XMVECTOR m_v;
        float fValues[4];
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