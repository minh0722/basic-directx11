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
    	
    DirectX::XMMATRIX GetViewMatrix();
	DirectX::XMMATRIX GetProjectionMatrix();

private:
	void UpdateCameraMatrices();

private:
	DirectX::XMVECTOR m_ForwardOrientation;
	DirectX::XMVECTOR m_RightOrientation;
	DirectX::XMVECTOR m_UpOrientation;

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