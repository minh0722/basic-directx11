#pragma once
#include "pch.h"

class Vector4f
{
public:
    float& operator[](uint16_t index);
    float operator[](uint16_t index) const;

private:
    union
    {
        XMVECTOR v;
        float fValues[4];
    };
};