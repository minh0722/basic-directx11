#include "pch.h"
#include "GraphicsComponent.h"

GraphicsComponent::GraphicsComponent(GraphicsComponentDesc& desc)
{
	InitVertexShader(desc.device, desc.vertexShaderFilePath);
	InitPixelShader(desc.device, desc.pixelShaderFilePath);
	InitVertexInputLayout(desc.device, desc.vertexShaderFilePath, desc.vertexInputLayout);
}


GraphicsComponent::~GraphicsComponent()
{
}

void GraphicsComponent::Render(ID3D11DeviceContext* context)
{

}

void GraphicsComponent::InitIndexBuffer(ID3D11Device* device, std::vector<uint32_t>& indices)
{
	size_t indicesCount = indices.size();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = indicesCount * sizeof(uint32_t);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(uint32_t);
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices.data();

	THROW_IF_FAILED(
		device->CreateBuffer(
			&desc,
			&initData,
			m_IndexBuffer.GetAddressOf()));
}

void GraphicsComponent::InitVertexBuffer(ID3D11Device* device, std::vector<Vertex>& vertices)
{
	size_t verticesCount = vertices.size();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = verticesCount * sizeof(Vertex);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(Vertex);
	desc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	THROW_IF_FAILED(
		device->CreateBuffer(
			&desc,
			&initData,
			m_VertexBuffer.GetAddressOf()));
}

void GraphicsComponent::InitVertexShader(ID3D11Device* device, LPCWSTR filePath)
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

void GraphicsComponent::InitPixelShader(ID3D11Device* device, LPCWSTR filePath)
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

void GraphicsComponent::InitVertexInputLayout(ID3D11Device* device, LPCWSTR filePath, std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
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
