#include "Camera.h"
#include "Matrix44f.h"

Camera::Camera()
{

}

void Camera::SetTranslation(const Vector4f& translation)
{
    Matrix44f translationMatrix;
    translationMatrix[0][3] = translation[0];
    translationMatrix[1][3] = translation[1];
    translationMatrix[2][3] = translation[2];
    translationMatrix[3][3] = translation[3];

    m_WorldMatrix *= translationMatrix;
}

void Camera::SetRotation(Axis axis, float degree)
{
	XMMATRIX rotationMatrix;
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
    scaleMatrix[0][0] = scale[0];
    scaleMatrix[1][1] = scale[1];
    scaleMatrix[2][2] = scale[2];
    scaleMatrix[3][3] = scale[3];

	m_WorldMatrix *= scaleMatrix;
}

Matrix44f Camera::GetViewMatrix() const
{
    return Matrix44f(XMMatrixInverse(nullptr, m_WorldMatrix.GetMatrixComponent()));
}