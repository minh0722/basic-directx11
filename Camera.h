#pragma once
#include "pch.h"
#include "Matrix44f.h"

class Vector4f;
class Matrix44f;

class Camera
{
public:
	Camera();
    void SetTranslation(const Vector4f& translation);
    void SetRotation(Axis axis, float degree);
    void SetScale(const Vector4f& scale);

	Matrix44f GetViewMatrix() const;
private:
    Matrix44f m_WorldMatrix;
};