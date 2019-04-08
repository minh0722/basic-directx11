#pragma once
#include <type_traits>

template <typename T>
class Vector2
{
	static_assert(std::is_arithmetic<T>::value);

public:
	Vector2() = default;
	Vector2(T x, T y);
	Vector2(const Vector2& other);
	
	Vector2& operator=(const Vector2& other);
	Vector2 operator-(const Vector2& other) const;
    Vector2 operator+(const Vector2& other) const;
	Vector2 operator/(T num) const;
    Vector2 operator*(T num) const;
	Vector2& operator*=(const T num);
	Vector2& operator+=(const T num);

	T DotProduct(const Vector2& other) const;

	T& operator[](uint16_t index);
	T operator[](uint16_t index) const;

    bool operator==(const Vector2& other) const;
    bool operator!=(const Vector2& other) const;
    
    Vector2<float> ToFloatVec() const;

public:
	union
	{
		T m_Values[2] = {};
		struct
		{
			T x;
			T y;
		};
	};
};

template <typename T>
Vector2<T>::Vector2(T x, T y)
{
	m_Values[0] = x;
	m_Values[1] = y;
}

template <typename T>
Vector2<T>::Vector2(const Vector2& other)
{
	m_Values[0] = other.m_Values[0];
	m_Values[1] = other.m_Values[1];
}

template <typename T>
Vector2<T>& Vector2<T>::operator=(const Vector2<T>& other)
{
	if (this != &other)
	{
		m_Values[0] = other.m_Values[0];
		m_Values[1] = other.m_Values[1];
	}

	return *this;
}

template <typename T>
Vector2<T> Vector2<T>::operator-(const Vector2<T>& other) const
{
	return Vector2(
		m_Values[0] - other.m_Values[0],
		m_Values[1] - other.m_Values[1]);
}

template <typename T>
Vector2<T> Vector2<T>::operator+(const Vector2<T>& other) const
{
    return Vector2(
        m_Values[0] + other.m_Values[0],
        m_Values[1] + other.m_Values[1]
    );
}

template <typename T>
Vector2<T> Vector2<T>::operator/(T num) const
{
	return Vector2<T>(
		m_Values[0] / num,
		m_Values[1] / num);
}

template <typename T>
Vector2<T> Vector2<T>::operator*(T num) const
{
    return Vector2<T>(
        m_Values[0] * num,
        m_Values[1] * num);
}

template <typename T>
Vector2<T>& Vector2<T>::operator*=(const T num)
{
	m_Values[0] *= num;
	m_Values[1] *= num;

	return *this;
}

template <typename T>
Vector2<T>& Vector2<T>::operator+=(const T num)
{
	m_Values[0] += num;
	m_Values[1] += num;

	return *this;
}

template <typename T>
T Vector2<T>::DotProduct(const Vector2& other) const
{
	return m_Values[0] * other.m_Values[0] +
		m_Values[1] * other.m_Values[1];
}

template <typename T>
T& Vector2<T>::operator[](uint16_t index)
{
	assert(index < 2);
	return m_Values[index];
}

template <typename T>
T Vector2<T>::operator[](uint16_t index) const
{
	assert(index < 2);
	return m_Values[index];
}

template <typename T>
bool Vector2<T>::operator==(const Vector2& other) const
{
    return m_Values[0] == other.m_Values[0] && 
           m_Values[1] == other.m_Values[1];
}

template <typename T>
bool Vector2<T>::operator!=(const Vector2& other) const
{
    return !(this->operator==(other));
}

template <typename T>
Vector2<float> Vector2<T>::ToFloatVec() const
{    
    return Vector2<float>((float)m_Values[0], (float)m_Values[1]);
}