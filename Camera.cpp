#include "Camera.h"

Camera::Camera()
	: m_CurrentOrientationForwardDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_CurrentOrientationRightDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_CurrentOrientationUpDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
    , m_ForwardMovementDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
    , m_RightMovementDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
    , m_UpMovementDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
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
	: m_CurrentOrientationForwardDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
	, m_CurrentOrientationRightDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
	, m_CurrentOrientationUpDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
    , m_ForwardMovementDirection(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f))
    , m_RightMovementDirection(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f))
    , m_UpMovementDirection(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
	, m_Fov(fov)
	, m_NearPlaneDist(0.1f)
	, m_FarPlaneDist(1000.0f)
	, m_Position(position)
	, m_RollPitchYawRotationMatrix(DirectX::XMMatrixIdentity())
	, m_NeedToUpdateMatrices(true)
{
	m_LookAt = DirectX::XMVectorAdd(m_Position, m_CurrentOrientationForwardDirection);
	UpdateCameraMatrices();
}

void Camera::MoveCameraWorldAxisAligned(const DirectX::XMVECTOR& moveVector)
{
	float xDirectionAmount = DirectX::XMVectorGetX(moveVector);		// left right
	float yDirectionAmount = DirectX::XMVectorGetY(moveVector);		// up down
	float zDirectionAmount = DirectX::XMVectorGetZ(moveVector);		// forward backward

    // only move the camera in the movement directional vectors
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_RightMovementDirection, xDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_UpMovementDirection, yDirectionAmount));
	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_ForwardMovementDirection, zDirectionAmount));
	
	m_NeedToUpdateMatrices = true;
}

void Camera::MoveCameraOrientationAxisAligned(const DirectX::XMVECTOR& direction)
{
    float xDirectionAmount = DirectX::XMVectorGetX(direction);		// left right
    float yDirectionAmount = DirectX::XMVectorGetY(direction);		// up down
    float zDirectionAmount = DirectX::XMVectorGetZ(direction);		// forward backward

                                                                    // only move the camera in the movement directional vectors
    m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_CurrentOrientationRightDirection, xDirectionAmount));
    m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_CurrentOrientationUpDirection, yDirectionAmount));
    m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_CurrentOrientationForwardDirection, zDirectionAmount));

    m_NeedToUpdateMatrices = true;
}

void Camera::Rotate(RotationAxis axis, float degree)
{
    float angleToRotate = degree * RADIAN;

	switch (axis)
	{
	case Roll:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(m_CurrentOrientationForwardDirection, angleToRotate);

		m_CurrentOrientationRightDirection = DirectX::XMVector3Transform(m_CurrentOrientationRightDirection, m_RollPitchYawRotationMatrix);
		m_CurrentOrientationUpDirection = DirectX::XMVector3Transform(m_CurrentOrientationUpDirection, m_RollPitchYawRotationMatrix);

		break;
	case Yaw:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(m_UpMovementDirection, angleToRotate);

		m_CurrentOrientationForwardDirection = DirectX::XMVector3Transform(m_CurrentOrientationForwardDirection, m_RollPitchYawRotationMatrix);
		m_CurrentOrientationRightDirection = DirectX::XMVector3Transform(m_CurrentOrientationRightDirection, m_RollPitchYawRotationMatrix);
        m_CurrentOrientationUpDirection = DirectX::XMVector3Transform(m_CurrentOrientationUpDirection, m_RollPitchYawRotationMatrix);

        m_ForwardMovementDirection = DirectX::XMVector3Transform(m_ForwardMovementDirection, m_RollPitchYawRotationMatrix);
        m_RightMovementDirection = DirectX::XMVector3Transform(m_RightMovementDirection, m_RollPitchYawRotationMatrix);

		break;
	case Pitch:
		m_RollPitchYawRotationMatrix = DirectX::XMMatrixRotationAxis(m_RightMovementDirection, angleToRotate);

		m_CurrentOrientationForwardDirection = DirectX::XMVector3Transform(m_CurrentOrientationForwardDirection, m_RollPitchYawRotationMatrix);
		m_CurrentOrientationUpDirection = DirectX::XMVector3Transform(m_CurrentOrientationUpDirection, m_RollPitchYawRotationMatrix);
        m_CurrentOrientationRightDirection = DirectX::XMVector3Transform(m_CurrentOrientationRightDirection, m_RollPitchYawRotationMatrix);

		break;
	default:
		THROW_IF_FAILED(E_FAIL);
		break;
	}

	// update the lookat position by adding the current position with the forward directional vector in the current camera orientation
	m_LookAt = DirectX::XMVectorAdd(m_Position, m_CurrentOrientationForwardDirection);

	OUTPUT_DEBUG("Lookat = %f %f %f %f\n", m_CurrentOrientationForwardDirection.m128_f32[0], m_CurrentOrientationForwardDirection.m128_f32[1], m_CurrentOrientationForwardDirection.m128_f32[2], m_CurrentOrientationForwardDirection.m128_f32[3]);
	
	m_NeedToUpdateMatrices = true;
}

void Camera::ZoomCamera(ZoomType zoom, int zoomThreshHold)
{
	float zoomCoeff = zoom == ZoomType::ZoomIn ? 1.0f : -1.0f;

	m_Position = DirectX::XMVectorAdd(m_Position, DirectX::XMVectorScale(m_CurrentOrientationForwardDirection, zoomCoeff * 1.0f));
	m_NeedToUpdateMatrices = true;

	//OUTPUT_DEBUG("Camera position: %f %f %f %f\n", m_Position.m128_f32[0], m_Position.m128_f32[1], m_Position.m128_f32[2], m_Position.m128_f32[3]);
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
		m_LookAt = DirectX::XMVectorAdd(m_Position, m_CurrentOrientationForwardDirection);

		m_ViewMatrix = DirectX::XMMatrixLookAtLH(m_Position, m_LookAt, m_CurrentOrientationUpDirection);
		m_PerspectiveProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_Fov * RADIAN, (float)screenWidth / (float)screenHeight, m_NearPlaneDist, m_FarPlaneDist);

		m_NeedToUpdateMatrices = false;
	}
}