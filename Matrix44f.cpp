#include "Matrix44f.h"

Matrix44f::Matrix44f()
{
    m_matrix = XMMatrixIdentity();
}

Matrix44f::Matrix44f(const XMMATRIX& other)
{
    m_matrix = other;
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

Matrix44f& Matrix44f::operator*=(const XMMATRIX& other)
{
    m_matrix *= other;

    return *this;
}

Matrix44f Matrix44f::GetInverseMatrix()
{
    XMMATRIX input(&m_elements[0][0]);
    input = XMMatrixInverse(nullptr, input);

    Matrix44f result;
    memcpy(result.m_elements, &input.r[0], 16 * sizeof(float));

    return result;
}

XMMATRIX Matrix44f::GetMatrixComponent() const
{
    return m_matrix;
}
