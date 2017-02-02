#pragma once
#include "pch.h"

class Camera
{
public:
    void SetTranslation(const Vector4f& translation);
    void SetRotation(Axis axis, float degree);
    void SetScale(const Vector4f& scale);
private:
    Matrix44f m_WorldMatrix;

};