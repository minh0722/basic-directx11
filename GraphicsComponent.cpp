#include "pch.h"
#include "GraphicsComponent.h"

GraphicsComponent::GraphicsComponent(GraphicsComponentDesc& desc)
{
	initVertexShader(desc.device, desc.vertexShaderFilePath);
	initPixelShader(desc.device, desc.pixelShaderFilePath);
	initVertexInputLayout(desc.device, desc.vertexShaderFilePath, desc.vertexInputLayout);
}


GraphicsComponent::~GraphicsComponent()
{
}

void GraphicsComponent::render()
{
}

void GraphicsComponent::initVertexShader(ID3D11Device* device, LPCWSTR filePath)
{
	ID3DBlob* blob;
	THROW_IF_FAILED(D3DReadFileToBlob(filePath, &blob));

	THROW_IF_FAILED(
		device->CreateVertexShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_VertexShader.GetAddressOf()));
}

void GraphicsComponent::initPixelShader(ID3D11Device* device, LPCWSTR filePath)
{
	ID3DBlob* blob;
	THROW_IF_FAILED(D3DReadFileToBlob(filePath, &blob));

	THROW_IF_FAILED(
		device->CreatePixelShader(
			blob->GetBufferPointer(),
			blob->GetBufferSize(),
			nullptr,
			m_PixelShader.GetAddressOf()));
}

void GraphicsComponent::initVertexInputLayout(ID3D11Device* device, LPCWSTR filePath, std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
{
	ID3DBlob* vertexBlob;
	THROW_IF_FAILED(D3DReadFileToBlob(filePath, &vertexBlob));

	THROW_IF_FAILED(
		device->CreateInputLayout(
			inputLayoutDesc.data(),
			inputLayoutDesc.size(),
			vertexBlob->GetBufferPointer(),
			vertexBlob->GetBufferSize(),
			m_VertexInputLayout.GetAddressOf()));
}
