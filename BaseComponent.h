#pragma once
#include "pch.h"

class BaseComponent
{
public:
	virtual ~BaseComponent() {};

	virtual void Render(ID3D11DeviceContext* context) = 0;

	// TODO: might make these template
	virtual void SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices) {};
	virtual void SetVertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices) {};
};

