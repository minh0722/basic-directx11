#include "Camera.h"

Camera::Camera()
	: m_ViewMatrix(DirectX::XMMatrixIdentity())
	, m_PerspectiveProjectionMatrix(DirectX::XMMatrixIdentity())
	, m_RotationMatrixX(DirectX::XMMatrixIdentity())
	, m_RotationMatrixY(DirectX::XMMatrixIdentity())
	, m_RotationMatrixZ(DirectX::XMMatrixIdentity())
	, m_Position(DirectX::XMVectorZero())
	, m_LookAt(DirectX::XMVectorSet(1.0f, 1.0f, 1.0f, 0.0f))
	, m_UpDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
{
}

Camera::Camera(const DirectX::XMVECTOR& position, const DirectX::XMVECTOR& lookAt, const DirectX::XMVECTOR& upDirection)
	: m_Position(position), m_LookAt(lookAt), m_UpDirection(upDirection)
{
}

void Camera::MoveCamera(const DirectX::XMVECTOR& moveVector)
{
	m_Position = DirectX::XMVectorAdd(m_Position, moveVector);
}

void Camera::SetRotation(Axis axis, float degree)
{
	switch (axis)
	{
	case X:
		m_RotationMatrixX = DirectX::XMMatrixRotationX(degree * RADIAN);
		break;
	case Y:
		m_RotationMatrixY = DirectX::XMMatrixRotationY(degree * RADIAN);
		break;
	case Z:
		m_RotationMatrixZ = DirectX::XMMatrixRotationZ(degree * RADIAN);
		break;
	default:
		THROW_IF_FAILED(E_FAIL);
		break;
	}
}

void Camera::SetLookAt(const DirectX::XMVECTOR& lookAt)
{
	m_LookAt = lookAt;
}

void Camera::SetCameraPosition(const DirectX::XMVECTOR& position)
{
    m_Position = position;
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

DirectX::XMMATRIX Camera::GetViewMatrix() const
{
	return m_ViewMatrix;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix() const
{
	return m_PerspectiveProjectionMatrix;
}

void Camera::UpdateCameraMatrices()
{
	m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_LookAt, m_UpDirection);
	m_PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov * RADIAN, (float)screenWidth / (float)screenHeight, m_NearPlaneDist, m_FarPlaneDist);
}