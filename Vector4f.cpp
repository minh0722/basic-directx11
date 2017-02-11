#include "Vector4f.h"
#include "Matrix44f.h"

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

Vector4f Vector4f::operator-(const Vector4f& other) const
{
    return Vector4f(
        fValues[0] - other.fValues[0], 
        fValues[1] - other.fValues[1], 
        fValues[2] - other.fValues[2], 
        fValues[3] - other.fValues[3]);
}

Vector4f Vector4f::operator/(float num) const
{
    return Vector4f(
        fValues[0] / num,
        fValues[1] / num,
        fValues[2] / num,
        fValues[3] / num);
}

Vector4f Vector4f::operator*(const Matrix44f& other) const
{
    return Vector4f(
        x * other[0][0] + y * other[1][0] + z * other[2][0],
        x * other[0][1] + y * other[1][1] + z * other[2][1],
        x * other[0][2] + y * other[1][2] + z * other[2][2],
        1.0f);
}

Vector4f Vector4f::CrossProduct(const Vector4f& other) const
{
    return Vector4f(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x,
		1.0f);
}

float Vector4f::DotProduct(const Vector4f& other) const
{
	return fValues[0] * other.fValues[0] +
		fValues[1] * other.fValues[1] +
		fValues[2] * other.fValues[2] +
		fValues[3] * other.fValues[3];
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

float Vector4f::GetLength() const
{
    Vector4f res(XMVector3Length(m_v));
    return res.fValues[0];
}