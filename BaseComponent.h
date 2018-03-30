#pragma once
#include "pch.h"

struct Vertex;

class BaseComponent
{
public:
	virtual ~BaseComponent() {};

	virtual void Render(ID3D11DeviceContext* context, bool isInstanceRendering = false, uint32_t instanceCount = 1) = 0;

	// TODO: might make these template
	virtual void SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices) {};
	virtual void SetIndexBuffer(ID3D11Device* device, const void* indices, uint32_t indicesCount) {};
	virtual void SetVertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices) {};
	virtual void SetVertexBuffer(ID3D11Device* device, const std::vector<std::tuple<float, float, float>>& vertices) {};
	virtual void SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology) {};
};

