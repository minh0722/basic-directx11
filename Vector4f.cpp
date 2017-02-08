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

Vector4f::Vector4f(const XMVECTOR& other)
{
    m_v = other;
}

Vector4f& Vector4f::operator=(const Vector4f& other)
{
	if (this != &other)
	{
		fValues[0] = other.fValues[0];
		fValues[1] = other.fValues[1];
		fValues[2] = other.fValues[2];
		fValues[3] = other.fValues[3];
	}

	return *this;
}

Vector4f Vector4f::operator-(const Vector4f& other)
{
    return Vector4f(
        fValues[0] - other.fValues[0], 
        fValues[1] - other.fValues[1], 
        fValues[2] - other.fValues[2], 
        fValues[3] - other.fValues[3]);
}

Vector4f Vector4f::operator/(float num)
{
    return Vector4f(
        fValues[0] / num,
        fValues[1] / num,
        fValues[2] / num,
        fValues[3] / num);
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

float Vector4f::DotProduct(const Vector4f& other)
{
	return fValues[0] * other.fValues[0] + 
		fValues[1] * other.fValues[1] + 
		fValues[2] * other.fValues[2] + 
		fValues[3] * other.fValues[3];
}

float Vector4f::GetLength() const
{
    Vector4f res(XMVector3Length(m_v));
    return res.fValues[0];
}