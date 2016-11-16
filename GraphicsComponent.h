#pragma once
#include "pch.h"

class GraphicsComponent
{
public:
	struct GraphicsComponentDesc
	{
		ID3D11Device* device;
		LPCWSTR vertexShaderFilePath;
		LPCWSTR pixelShaderFilePath;
		std::vector<D3D11_INPUT_ELEMENT_DESC> vertexInputLayout;
	};

public:
	GraphicsComponent(GraphicsComponentDesc& desc);
	~GraphicsComponent();
	
	void render();

protected:

	void initVertexShader(ID3D11Device* device, LPCWSTR filePath);
	void initPixelShader(ID3D11Device* device, LPCWSTR filePath);
	void initVertexInputLayout(
		ID3D11Device* device,
		LPCWSTR filePath,
		std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc);

private:
	ComPtr<ID3D11VertexShader> m_VertexShader;
	ComPtr<ID3D11PixelShader> m_PixelShader;

	ComPtr<ID3D11Buffer> m_VertexBuffer;
	ComPtr<ID3D11Buffer> m_IndexBuffer;

	ComPtr<ID3D11InputLayout> m_VertexInputLayout;
};