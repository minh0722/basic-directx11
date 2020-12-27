#include "pch.h"
#include "Matrix44f.h"

Matrix44f::Matrix44f()
{
    m_matrix = DirectX::XMMatrixIdentity();
}

Matrix44f::Matrix44f(const DirectX::XMMATRIX& other)
{
    m_matrix = other;
}

Matrix44f::Matrix44f(const Matrix44f& other)
{
	m_matrix = other.m_matrix;
}

Matrix44f& Matrix44f::operator=(const Matrix44f& other)
{
    if(this != &other)
    {
        m_matrix = other.m_matrix;
    }
    return *this;
}

Vector4f& Matrix44f::operator[](uint16_t rowIndex)
{
    return m_rows[rowIndex];
}

const Vector4f& Matrix44f::operator[](uint16_t rowIndex) const
{
    return m_rows[rowIndex];
}

Matrix44f& Matrix44f::operator*=(const Matrix44f& other)
{
    m_matrix *= other.m_matrix;

    return *this;
}

Matrix44f& Matrix44f::operator*=(const DirectX::XMMATRIX& other)
{
    m_matrix *= other;

    return *this;
}

Vector4f Matrix44f::operator*(const Vector4f& v)
{
	return { m_rows[0].DotProduct(v), m_rows[1].DotProduct(v), m_rows[2].DotProduct(v), m_rows[3].DotProduct(v) };
}

Matrix44f Matrix44f::operator*(const Matrix44f& other)
{
	return{ m_matrix * other.m_matrix };
}

Matrix44f Matrix44f::GetInverseMatrix()
{
	DirectX::XMMATRIX input(&m_elements[0][0]);
    input = DirectX::XMMatrixInverse(nullptr, input);

    Matrix44f result;
    memcpy(result.m_elements, &input.r[0], 16 * sizeof(float));

    return result;
}

DirectX::XMMATRIX Matrix44f::GetMatrixComponent() const
{
    return m_matrix;
}

Vector4f* Matrix44f::GetRows()
{
	return &m_rows[0];
}
