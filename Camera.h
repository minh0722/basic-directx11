#pragma once
#include "pch.h"
#include "Vector4f.h"


class Camera
{
public:
	Camera();
    Camera(const DirectX::XMVECTOR& worldPosition, const float fov);
    void MoveCamera(const DirectX::XMVECTOR& direction);
    void Rotate(RotationAxis axis, float degree);
    
	DirectX::XMVECTOR GetCameraPosition() const;
	DirectX::XMVECTOR GetCameraLookAt() const;
	float GetFOV() const;

    void SetLookAt(const DirectX::XMVECTOR& lookAt);
    void SetCameraPosition(const DirectX::XMVECTOR& position);
    void SetFov(const float fov);
    void SetNearPlaneDist(const float nearPlaneDist);
    void SetFarPlaneDist(const float farPlaneDist);

	DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix();

private:
	void UpdateCameraMatrices();

private:
	DirectX::XMVECTOR m_ForwardDirection;
	DirectX::XMVECTOR m_RightDirection;
	DirectX::XMVECTOR m_UpDirection;

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