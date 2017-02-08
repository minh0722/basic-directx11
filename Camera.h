#pragma once
#include "pch.h"
#include "Matrix44f.h"
#include "Vector4f.h"


class Camera
{
public:
	Camera();
    Camera(const Vector4f& worldPosition, const Vector4f& lookAt, const Vector4f& upAxis);
    void SetTranslation(const Vector4f& translation);
    void SetRotation(Axis axis, float degree);
    void SetScale(const Vector4f& scale);

    void SetLookAtPosition(const Vector4f& lookAtPosition);
    void SetCameraPosition(const Vector4f& position);
    void SetUpAxis(const Vector4f& upAxis);
    void SetFov(const float fov);
    void SetNearPlaneDist(const float nearPlaneDist);
    void SetFarPlaneDist(const float farPlaneDist);

	Matrix44f GetViewMatrix() const;
private:
    Matrix44f m_ViewMatrix;

    Vector4f m_LookAtVector;
    Vector4f m_Position;
    Vector4f m_UpAxis;
    float m_Fov;
    float m_NearPlaneDist;
    float m_FarPlaneDist;
};