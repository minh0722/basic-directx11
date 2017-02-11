#include "Camera.h"
#include "Matrix44f.h"

Camera::Camera()
{
    m_UpAxis = Vector4f(0.0f, 1.0f, 0.0f, 1.0f);
}

Camera::Camera(const Vector4f& worldPosition, const Vector4f& lookAt, const Vector4f& upAxis)
{
    m_Position = worldPosition;
    m_LookAtVector = lookAt;
    m_UpAxis = upAxis;
}

void Camera::SetTranslation(const Vector4f& translation)
{
    Matrix44f translationMatrix;
    translationMatrix[0][3] = translation[0];
    translationMatrix[1][3] = translation[1];
    translationMatrix[2][3] = translation[2];
    translationMatrix[3][3] = translation[3];

    m_ViewMatrix *= translationMatrix;
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

	m_ViewMatrix *= rotationMatrix;
}

void Camera::SetScale(const Vector4f& scale)
{
	Matrix44f scaleMatrix;
    scaleMatrix[0][0] = scale[0];
    scaleMatrix[1][1] = scale[1];
    scaleMatrix[2][2] = scale[2];
    scaleMatrix[3][3] = scale[3];

	m_ViewMatrix *= scaleMatrix;
}

void Camera::SetLookAtPosition(const Vector4f& lookAtPosition)
{
    Vector4f diff = lookAtPosition - m_Position;
    float diffLength = diff.GetLength();

    m_LookAtVector = diff / diffLength;
}

void Camera::SetCameraPosition(const Vector4f& position)
{
    m_Position = position;
}

void Camera::SetUpAxis(const Vector4f& upAxis)
{
    m_UpAxis = upAxis;
}

void Camera::SetFov(const float fov)
{
    m_Fov = fov;
}

void Camera::SetNearPlaneDist(const float nearPlaneDist)
{
    m_NearPlaneDist = nearPlaneDist;
}

void Camera::SetFarPlaneDist(const float farPlaneDist)
{
    m_FarPlaneDist = farPlaneDist;
}

Matrix44f Camera::GetViewMatrix() const
{
    //Vector4f ywCrossProduct = m_UpAxis.CrossProduct(m_LookAtVector);

    //Vector4f u = ywCrossProduct / ywCrossProduct.GetLength();

    //Vector4f v = m_LookAtVector.CrossProduct(u);
}