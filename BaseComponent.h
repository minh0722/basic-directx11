#pragma once
#include "pch.h"
#include "Vector3.h"

struct Vertex;

class BaseComponent
{
public:
	virtual ~BaseComponent() {};

	virtual void Render(ID3D11DeviceContext* context, bool isInstanceRendering = false, uint32_t instanceCount = 1) = 0;

	// TODO: might make these template
	virtual void SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices) {};
	virtual void SetIndexBuffer(ID3D11Device* device, const void* indices, size_t indicesCount) {};
	virtual void SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology) {};
};

