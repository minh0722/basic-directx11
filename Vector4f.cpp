#include "Vector4f.h"

Vector4f::Vector4f(float x, float y, float z, float w)
{
	fValues[0] = x;
	fValues[1] = y;
	fValues[2] = z;
	fValues[3] = w;
}

Vector4f::Vector4f(const Vector4f& other)
{
	fValues[0] = other.fValues[0];
	fValues[1] = other.fValues[1];
	fValues[2] = other.fValues[2];
	fValues[3] = other.fValues[3];
}

float& Vector4f::operator[](uint16_t index)
{
    assert(index < 4);
    return fValues[index];
}

float Vector4f::operator[](uint16_t index) const
{
    assert(index < 4);
    return fValues[index];
}