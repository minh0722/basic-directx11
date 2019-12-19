#include "Vector4f.h"
#include "Matrix44f.h"
#include "Vector3.h"

Vector4f::Vector4f(const Vector3<float>& v, float w)
{
	m_fValues[0] = v.x;
	m_fValues[1] = v.y;
	m_fValues[2] = v.z;
	m_fValues[2] = w;
}

Vector4f::Vector4f(float x, float y, float z, float w)
{
	m_fValues[0] = x;
	m_fValues[1] = y;
	m_fValues[2] = z;
	m_fValues[3] = w;
}

Vector4f::Vector4f(const Vector4f& other)
{
	m_fValues[0] = other.m_fValues[0];
	m_fValues[1] = other.m_fValues[1];
	m_fValues[2] = other.m_fValues[2];
	m_fValues[3] = other.m_fValues[3];
}

Vector4f::Vector4f(const DirectX::XMVECTOR& other)
{
    m_v = other;
}

Vector4f& Vector4f::operator=(const Vector4f& other)
{
	if (this != &other)
	{
		m_fValues[0] = other.m_fValues[0];
		m_fValues[1] = other.m_fValues[1];
		m_fValues[2] = other.m_fValues[2];
		m_fValues[3] = other.m_fValues[3];
	}

	return *this;
}

Vector4f Vector4f::operator-(const Vector4f& other) const
{
    return Vector4f(
        m_fValues[0] - other.m_fValues[0], 
        m_fValues[1] - other.m_fValues[1], 
        m_fValues[2] - other.m_fValues[2], 
        m_fValues[3] - other.m_fValues[3]);
}

Vector4f Vector4f::operator/(float num) const
{
    return Vector4f(
        m_fValues[0] / num,
        m_fValues[1] / num,
        m_fValues[2] / num,
        m_fValues[3] / num);
}

Vector4f Vector4f::operator*(const Matrix44f& other) const
{
    return Vector4f(
        x * other[0][0] + y * other[1][0] + z * other[2][0] + w * other[3][0],
        x * other[0][1] + y * other[1][1] + z * other[2][1] + w * other[3][1],
        x * other[0][2] + y * other[1][2] + z * other[2][2] + w * other[3][2],
        x * other[0][3] + y * other[1][3] + z * other[2][3] + w * other[3][3]);
}

Vector4f& Vector4f::operator*=(const float num)
{
    m_fValues[0] *= num;
    m_fValues[1] *= num;
    m_fValues[2] *= num;
    m_fValues[3] *= num;

    return *this;
}

Vector4f& Vector4f::operator+=(const float num)
{
    m_fValues[0] += num;
    m_fValues[1] += num;
    m_fValues[2] += num;
    m_fValues[3] += num;

    return *this;
}

bool Vector4f::operator==(const Vector4f& other) const
{
    return x == other.x && y == other.y && z == other.z && w == other.w;
}

bool Vector4f::operator!=(const Vector4f& other) const
{
    return !(*this == other);
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
	return m_fValues[0] * other.m_fValues[0] +
		m_fValues[1] * other.m_fValues[1] +
		m_fValues[2] * other.m_fValues[2] +
		m_fValues[3] * other.m_fValues[3];
}

float Vector4f::DotProduct3(const Vector4f& other) const
{
    return m_fValues[0] * other.m_fValues[0] +
        m_fValues[1] * other.m_fValues[1] +
        m_fValues[2] * other.m_fValues[2];
}

float& Vector4f::operator[](uint16_t index)
{
    assert(index < 4);
    return m_fValues[index];
}

float Vector4f::operator[](uint16_t index) const
{
    assert(index < 4);
    return m_fValues[index];
}

float Vector4f::GetLength() const
{
    Vector4f res(DirectX::XMVector3Length(m_v));
    return res.m_fValues[0];
}

void Vector4f::Normalize3()
{
    m_v = DirectX::XMVector3Normalize(m_v);
}

void Vector4f::Mul3(const Vector4f& other)
{
    m_v = DirectX::XMVectorMultiply(m_v, other.m_v);
    m_fValues[3] = 1.0f;
}

Vector4f Vector4f::Mul3(const Vector4f& other) const
{
    Vector4f res(*this);
    res.Mul3(other);
    return res;
}

Vector4f Vector4f::Abs() const
{
    return Vector4f{
        abs(x),
        abs(y),
        abs(z),
        abs(w)
    };
}

Vector3<float> Vector4f::XYZ() const
{
    return Vector3<float>(x, y, z);
}