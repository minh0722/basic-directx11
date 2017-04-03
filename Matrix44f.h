#pragma once
#include "pch.h"
#include "Vector4f.h"

class Matrix44f
{
public:
    Matrix44f();
    Matrix44f(const XMMATRIX& other);
	Matrix44f(const Matrix44f& other);
    Matrix44f& operator=(const Matrix44f& other);

    Vector4f& operator[](uint16_t rowIndex);
    const Vector4f& operator[](uint16_t rowIndex) const;

    Matrix44f& operator*=(const Matrix44f& other);
    Matrix44f& operator*=(const XMMATRIX& other);

	Vector4f operator*(const Vector4f& v);
	Matrix44f operator*(const Matrix44f& other);

    Matrix44f GetInverseMatrix();

    XMMATRIX GetMatrixComponent() const;
	Vector4f* GetRows();

public:
    union
    {
        XMMATRIX m_matrix;
        Vector4f m_rows[4];
        float m_elements[4][4];
    };
};