#pragma once
#include "pch.h"

class BaseComponent
{
public:
	virtual ~BaseComponent() {};

	virtual void Render(ID3D11DeviceContext* context) = 0;
};

