#pragma once
#include "pch.h"

class GraphicsComponent
{
public:
	GraphicsComponent();
	~GraphicsComponent();

private:
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;

	ComPtr<ID3D11InputLayout> m_VertexInputLayout;
};

