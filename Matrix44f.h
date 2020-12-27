#pragma once
#include "Vector4f.h"

class Matrix44f
{
public:
    Matrix44f();
    Matrix44f(const DirectX::XMMATRIX& other);
	Matrix44f(const Matrix44f& other);
    Matrix44f& operator=(const Matrix44f& other);

    Vector4f& operator[](uint16_t rowIndex);
    const Vector4f& operator[](uint16_t rowIndex) const;

    Matrix44f& operator*=(const Matrix44f& other);
    Matrix44f& operator*=(const DirectX::XMMATRIX& other);

	Vector4f operator*(const Vector4f& v);
	Matrix44f operator*(const Matrix44f& other);

    Matrix44f GetInverseMatrix();

	DirectX::XMMATRIX GetMatrixComponent() const;
	Vector4f* GetRows();

public:
    union
    {
		DirectX::XMMATRIX m_matrix;
        Vector4f m_rows[4];
        float m_elements[4][4];
    };
};