#include "Camera.h"

Camera::Camera()
	: m_ForwardDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_RightDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_UpDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
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
	: m_ForwardDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_RightDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_UpDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	, m_Fov(fov)
	, m_NearPlaneDist(0.1f)
	, m_FarPlaneDist(100.0f)
	, m_Position(position)
	, m_RollPitchYawRotationMatrix(DirectX::XMMatrixIdentity())
	, m_NeedToUpdateMatrices(true)
{
	m_LookAt = DirectX::XMVectorAdd(m_Position, m_ForwardDirection);
	UpdateCameraMatrices();
}

void Camera::MoveCamera(const DirectX::XMVECTOR& moveVector)
{
	float xDirectionAmount = DirectX::XMVectorGetX(moveVector);		// left right
	float yDirectionAmount = DirectX::XMVectorGetY(moveVector);		// up down
	float zDirectionAmount = DirectX::XMVectorGetZ(moveVector);		// forward backward

	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_RightDirection, xDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_UpDirection, yDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_ForwardDirection, zDirectionAmount));
	
	m_NeedToUpdateMatrices = true;
}

void Camera::Rotate(RotationAxis axis, float degree)
{
	// obtain the directional vector from camera position to lookat
	DirectX::XMVECTOR lookatDir = DirectX::XMVectorSubtract(m_LookAt, m_Position);

	switch (axis)
	{
	case Roll:
		// calculate the roll pitch yaw matrix
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0, 0.0, degree * RADIAN);

		// rotate the directional vector
		lookatDir = DirectX::XMVector3Transform(lookatDir, m_RollPitchYawRotationMatrix);
		
		// update the lookat position by adding the current position with the directional vector
		m_LookAt = DirectX::XMVectorAdd(m_Position, lookatDir);
		break;
	case Yaw:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(0.0, degree * RADIAN, 0.0f);

		lookatDir = DirectX::XMVector3Transform(lookatDir, m_RollPitchYawRotationMatrix);
				
		m_LookAt = DirectX::XMVectorAdd(m_Position, lookatDir);
		
		break;
	case Pitch:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(degree * RADIAN, 0.0f, 0.0f);

		lookatDir = DirectX::XMVector3Transform(lookatDir, m_RollPitchYawRotationMatrix);

		m_LookAt = DirectX::XMVectorAdd(m_Position, lookatDir);
		break;
	default:
		THROW_IF_FAILED(E_FAIL);
		break;
	}

	char buf[256];
	snprintf(buf, 256, "Lookat = %f %f %f %f\n", m_LookAt.m128_f32[0], m_LookAt.m128_f32[1], m_LookAt.m128_f32[2], m_LookAt.m128_f32[3]);
	OutputDebugStringA(buf);

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
		m_LookAt = DirectX::XMVectorAdd(m_Position, m_ForwardDirection);

		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_LookAt, m_UpDirection);
		m_PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov * RADIAN, (float)screenWidth / (float)screenHeight, m_NearPlaneDist, m_FarPlaneDist);

		m_NeedToUpdateMatrices = false;
	}
}