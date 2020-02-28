#pragma once
#include "pch.h"

template <typename T>
class Vector3
{
	static_assert(std::is_arithmetic<T>::value);

public:
	Vector3<T>() = default;
	Vector3<T>(T x, T y, T z);
	Vector3<T>(const Vector3<T>& other);
    Vector3<T>(const DirectX::XMVECTOR& v);
	Vector3<T>& operator=(const Vector3<T>& other);
	Vector3<T> operator-(const Vector3<T>& other) const;
	Vector3<T> operator+(const Vector3<T>& other) const;
	Vector3<T> operator*(const float f) const;
	Vector3<T> operator/(T num) const;
	Vector3<T>& operator*=(const T num);
	Vector3<T>& operator+=(const T num);
	Vector3<T> operator-() const;
	Vector3<T> CrossProduct(const Vector3<T>& other) const;
	T DotProduct(const Vector3<T>& other) const;
    DirectX::XMVECTOR ToXMVector() const;

	T& operator[](uint16_t index);
	T operator[](uint16_t index) const;

	Vector3<T> Normalize();
	T Length();
	

public:
	union
	{
		T m_Values[3];
		struct
		{
			T x, y, z;
		};
		struct
		{
			T r, g, b;
		};
	};
};

template <typename T>
Vector3<T> abs(const Vector3<T>& v)
{
    return { abs(v.x), abs(v.y), abs(v.z) };
}

template <typename T>
T dot(T n, const Vector3<T>& v)
{
    return v.DotProduct({ n, n, n });
}

template <typename T>
Vector3<T>::Vector3(T x, T y, T z)
{
	m_Values[0] = x;
	m_Values[1] = y;
	m_Values[2] = z;
}

template <typename T>
Vector3<T>::Vector3(const Vector3<T>& other)
{
	m_Values[0] = other.m_Values[0];
	m_Values[1] = other.m_Values[1];
	m_Values[2] = other.m_Values[2];
}

template <typename T>
Vector3<T>::Vector3(const DirectX::XMVECTOR& v)
{
    x = DirectX::XMVectorGetX(v);
    y = DirectX::XMVectorGetY(v);
    z = DirectX::XMVectorGetZ(v);
}

template <typename T>
Vector3<T>& Vector3<T>::operator=(const Vector3<T>& other)
{
	if (this != &other)
	{
		m_Values[0] = other.m_Values[0];
		m_Values[1] = other.m_Values[1];
		m_Values[2] = other.m_Values[2];
	}

	return *this;
}

template <typename T>
Vector3<T> Vector3<T>::operator-(const Vector3<T>& other) const
{
	return Vector3<T>(
		m_Values[0] - other.m_Values[0],
		m_Values[1] - other.m_Values[1],
		m_Values[2] - other.m_Values[2]);
}

template <typename T>
Vector3<T> Vector3<T>::operator+(const Vector3<T>& other) const
{
	return Vector3<T>(
		m_Values[0] + other.m_Values[0],
		m_Values[1] + other.m_Values[1],
		m_Values[2] + other.m_Values[2]);
}

template <typename T>
Vector3<T> Vector3<T>::operator*(const float f) const
{
	return Vector3<T>(x * f, y * f, z * f);
}

template <typename T>
Vector3<T> Vector3<T>::operator/(T num) const
{
	return Vector3<T>(
		m_Values[0] / num,
		m_Values[1] / num,
		m_Values[2] / num);
}

template <typename T>
Vector3<T>& Vector3<T>::operator*=(const T num)
{
	m_Values[0] *= num;
	m_Values[1] *= num;
	m_Values[2] *= num;

	return *this;
}

template <typename T>
Vector3<T>& Vector3<T>::operator+=(const T num)
{
	m_Values[0] += num;
	m_Values[1] += num;
	m_Values[2] += num;

	return *this;
}

template <typename T>
Vector3<T> Vector3<T>::operator-() const
{
	return Vector3<float>(-x, -y, -z);
}

template <typename T>
Vector3<T> Vector3<T>::CrossProduct(const Vector3<T>& other) const
{
	return Vector3<T>(
		y * other.z - z * other.y,
		z * other.x - x * other.z,
		x * other.y - y * other.x,
		1.0f);
}

template <typename T>
T Vector3<T>::DotProduct(const Vector3<T>& other) const
{
	return m_Values[0] * other.m_Values[0] +
		m_Values[1] * other.m_Values[1] +
		m_Values[2] * other.m_Values[2];
}

template <typename T>
DirectX::XMVECTOR Vector3<T>::ToXMVector() const
{
    return DirectX::XMVectorSet(x, y, z, 1.0f);
}

template <typename T>
T& Vector3<T>::operator[](uint16_t index)
{
	assert(index < 3);
	return m_Values[index];
}

template <typename T>
T Vector3<T>::operator[](uint16_t index) const
{
	assert(index < 3);
	return m_Values[index];
}

template <typename T>
Vector3<T> Vector3<T>::Normalize()
{
	T len = Length();
	return Vector3(x / len, y / len, z / len);
}

template <typename T>
T Vector3<T>::Length()
{
	return sqrt(x * x + y * y + z * z);
}