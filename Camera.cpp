#include "Camera.h"

Camera::Camera()
	: m_ForwardOrientation(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_RightOrientation(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_UpOrientation(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	, m_ViewMatrix(DirectX::XMMatrixIdentity())
	, m_PerspectiveProjectionMatrix(DirectX::XMMatrixIdentity())
	, m_RollPitchYawRotationMatrix(DirectX::XMMatrixIdentity())
	, m_Position(DirectX::XMVectorZero())
	, m_LookAt(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_NeedToUpdateMatrices(true)
{
	UpdateCameraMatrices();
}

Camera::Camera(const DirectX::XMVECTOR& position, const float fov)
	: m_ForwardOrientation(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_RightOrientation(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_UpOrientation(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	, m_Fov(fov)
	, m_NearPlaneDist(0.1f)
	, m_FarPlaneDist(1000.0f)
	, m_Position(position)
	, m_RollPitchYawRotationMatrix(DirectX::XMMatrixIdentity())
	, m_NeedToUpdateMatrices(true)
{
	m_LookAt = DirectX::XMVectorAdd(m_Position, m_ForwardOrientation);
	UpdateCameraMatrices();
}

void Camera::MoveCamera(const DirectX::XMVECTOR& moveVector)
{
	float xDirectionAmount = DirectX::XMVectorGetX(moveVector);		// left right
	float yDirectionAmount = DirectX::XMVectorGetY(moveVector);		// up down
	float zDirectionAmount = DirectX::XMVectorGetZ(moveVector);		// forward backward

	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_RightOrientation, xDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_UpOrientation, yDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_ForwardOrientation, zDirectionAmount));
	
	m_NeedToUpdateMatrices = true;
}

void Camera::Rotate(RotationAxis axis, float degree)
{
	switch (axis)
	{
	case Roll:
		// calculate the roll pitch yaw matrix
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(m_ForwardOrientation, degree * RADIAN);

		// rotate the directional vectors
		m_RightOrientation = DirectX::XMVector3Transform(m_RightOrientation, m_RollPitchYawRotationMatrix);
		m_UpOrientation = DirectX::XMVector3Transform(m_UpOrientation, m_RollPitchYawRotationMatrix);

		break;
	case Yaw:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(DirectX::XMVectorSet(0.0, 1.0f, 0.0f, 0.0f), degree * RADIAN);

		m_ForwardOrientation = DirectX::XMVector3Transform(m_ForwardOrientation, m_RollPitchYawRotationMatrix);
		m_RightOrientation = DirectX::XMVector3Transform(m_RightOrientation, m_RollPitchYawRotationMatrix);
        m_UpOrientation = DirectX::XMVector3Transform(m_UpOrientation, m_RollPitchYawRotationMatrix);

		break;
	case Pitch:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(m_RightOrientation, degree * RADIAN);

		m_ForwardOrientation = DirectX::XMVector3Transform(m_ForwardOrientation, m_RollPitchYawRotationMatrix);
		m_UpOrientation = DirectX::XMVector3Transform(m_UpOrientation, m_RollPitchYawRotationMatrix);

		break;
	default:
		THROW_IF_FAILED(E_FAIL);
		break;
	}

	// update the lookat position by adding the current position with the directional vector
	m_LookAt = DirectX::XMVectorAdd(m_Position, m_ForwardOrientation);

	OUTPUT_DEBUG("Lookat = %f %f %f %f\n", m_ForwardOrientation.m128_f32[0], m_ForwardOrientation.m128_f32[1], m_ForwardOrientation.m128_f32[2], m_ForwardOrientation.m128_f32[3]);
	
	m_NeedToUpdateMatrices = true;
}

DirectX::XMVECTOR Camera::GetCameraPosition() const
{
	return m_Position;
}

DirectX::XMVECTOR Camera::GetCameraLookAt() const
{
	return m_LookAt;
}

float Camera::GetFOV() const
{
	return m_Fov;
}

void Camera::SetLookAt(const DirectX::XMVECTOR& lookAt)
{
	m_LookAt = lookAt;
	m_NeedToUpdateMatrices = true;
}

void Camera::SetCameraPosition(const DirectX::XMVECTOR& position)
{
    m_Position = position;
	m_NeedToUpdateMatrices = true;
}

void Camera::SetFov(const float fov)
{
    m_Fov = fov;
	m_NeedToUpdateMatrices = true;
}

void Camera::SetNearPlaneDist(const float nearPlaneDist)
{
    m_NearPlaneDist = nearPlaneDist;
	m_NeedToUpdateMatrices = true;
}

void Camera::SetFarPlaneDist(const float farPlaneDist)
{
    m_FarPlaneDist = farPlaneDist;
	m_NeedToUpdateMatrices = true;
}

DirectX::XMMATRIX Camera::GetViewMatrix()
{
	UpdateCameraMatrices();
	return m_ViewMatrix;
}

DirectX::XMMATRIX Camera::GetProjectionMatrix()
{
	UpdateCameraMatrices();
	return m_PerspectiveProjectionMatrix;
}

void Camera::UpdateCameraMatrices()
{
	if (m_NeedToUpdateMatrices)
	{
		m_LookAt = DirectX::XMVectorAdd(m_Position, m_ForwardOrientation);

		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_LookAt, m_UpOrientation);
		m_PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov * RADIAN, (float)screenWidth / (float)screenHeight, m_NearPlaneDist, m_FarPlaneDist);

		m_NeedToUpdateMatrices = false;
	}
}