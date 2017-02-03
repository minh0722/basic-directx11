#pragma once
#include "pch.h"

class Vector4f
{
public:
	Vector4f(float x, float y, float z, float w);
	Vector4f(const Vector4f& other);
	Vector4f& operator=(const Vector4f& other);
    float& operator[](uint16_t index);
    float operator[](uint16_t index) const;

	float DotProduct(const Vector4f& other);

private:
    union
    {
        XMVECTOR v;
        float fValues[4];
    };
};