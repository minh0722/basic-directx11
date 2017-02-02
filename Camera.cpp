#include "Camera.h"

void Camera::SetTranslation(const Vector4f& translation)
{
	Matrix44f translationMatrix;
	translationMatrix.SetColumn(3, translation);

	m_WorldMatrix *= translationMatrix;
}

void Camera::SetRotation(Axis axis, float degree)
{
	Matrix44f rotationMatrix;
	float radians = degree * RADIAN;
	float cosRes = std::cos(radians);
	float sinRes = std::sin(radians);

	switch (axis)
	{
	case X:
		rotationMatrix =
		{
			1.0f,	0.0f,	 0.0f,		0.0f,
			0.0f,	cosRes, -sinRes,	0.0f,
			0.0f,	sinRes,  cosRes,	0.0f,
			0.0f,	0.0f,	 0.0f,		1.0f
		};
		break;
	case Y:
		rotationMatrix =
		{
			cosRes, 0.0f,	 sinRes,	0.0f,
			0.0f,	1.0f,	 0.0f,		0.0f,
		   -sinRes, 0.0f,	 cosRes,	0.0f,
			0.0f,	0.0f,	 0.0f,		1.0f
		};
		break;
	case Z:
		rotationMatrix =
		{
			cosRes,	  -sinRes,	0.0f,	0.0f,
			sinRes,	   cosRes,	0.0f,	0.0f,
			0.0f,	   0.0f,	1.0f,	0.0f,
			0.0f,	   0.0f,	0.0f,	1.0f
		};
		break;
	case W:
		assert(false);
		break;
	default:
		break;
	}

	m_WorldMatrix *= rotationMatrix;
}

void Camera::SetScale(const Vector4f& scale)
{
	Matrix44f scaleMatrix;
	scaleMatrix.m_v[0].SetColumn(0, scale.nums[0]);
	scaleMatrix.m_v[1].SetColumn(1, scale.nums[0]);
	scaleMatrix.m_v[2].SetColumn(2, scale.nums[0]);
	scaleMatrix.m_v[3].SetColumn(3, scale.nums[0]);

	m_WorldMatrix *= scaleMatrix;
}

Matrix44f Camera::GetViewMatrix()
{

}