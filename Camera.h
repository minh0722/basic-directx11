#pragma once
#include "pch.h"
#include "Vector4f.h"


class Camera
{
public:
	Camera();
    Camera(const DirectX::XMVECTOR& worldPosition, const DirectX::XMVECTOR& lookAt, const DirectX::XMVECTOR& upAxis, const float fov);
    void MoveCamera(const DirectX::XMVECTOR& direction);
    void Rotate(Axis axis, float degree);
    
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
	DirectX::XMMATRIX m_ViewMatrix;
	DirectX::XMMATRIX m_PerspectiveProjectionMatrix;

	DirectX::XMVECTOR m_Position;
	DirectX::XMVECTOR m_LookAt;
	DirectX::XMVECTOR m_UpDirection;

	DirectX::XMMATRIX m_RotationMatrixX;
	DirectX::XMMATRIX m_RotationMatrixY;
	DirectX::XMMATRIX m_RotationMatrixZ;

	float m_Fov;
	float m_NearPlaneDist;
	float m_FarPlaneDist;

	bool m_NeedToUpdateMatrices;
};