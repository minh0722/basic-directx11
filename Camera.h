#pragma once
#include "pch.h"
#include "Vector4f.h"

enum ZoomType
{
	ZoomIn,
	ZoomOut
};

class Camera
{
public:
	Camera();
    Camera(const DirectX::XMVECTOR& worldPosition, const float fov);
    void MoveCameraWorldAxisAligned(const DirectX::XMVECTOR& direction);
    void MoveCameraOrientationAxisAligned(const DirectX::XMVECTOR& direction);
    void Rotate(RotationAxis axis, float degree);
	void ZoomCamera(ZoomType zoom, int zoomThreshHold = 1);
    	
    DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	void UpdateCameraMatrices();

private:
    // used for pitch and roll rotation
    // orientation of the camera changes in all rotations (roll, pitch, yaw)
	DirectX::XMVECTOR m_CurrentOrientationForwardDirection;
	DirectX::XMVECTOR m_CurrentOrientationRightDirection;
	DirectX::XMVECTOR m_CurrentOrientationUpDirection;

    // used for camera movement and yaw rotation
    DirectX::XMVECTOR m_ForwardMovementDirection;
    DirectX::XMVECTOR m_RightMovementDirection;
    DirectX::XMVECTOR m_UpMovementDirection;

	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_PerspectiveProjectionMatrix;

	DirectX::XMVECTOR m_Position;
	DirectX::XMVECTOR m_LookAt;

	DirectX::XMMATRIX m_RollPitchYawRotationMatrix;

	float m_Fov;
	float m_NearPlaneDist;
	float m_FarPlaneDist;

	bool m_NeedToUpdateMatrices;
};