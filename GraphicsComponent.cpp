#include "pch.h"
#include "GraphicsComponent.h"
#include "renderer.h"

GraphicsComponent::GraphicsComponent(const GraphicsComponentDesc& desc)
{
	InitVertexShader(desc.device, desc.vertexShaderFilePath);
	InitPixelShader(desc.device, desc.pixelShaderFilePath);
	InitVertexInputLayout(desc.device, desc.vertexShaderFilePath, desc.vertexInputLayout);

	// TODO: handle exceptions here...
}
static bool flag = false;
void GraphicsComponent::Render(ID3D11DeviceContext* context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	UINT startSlot = 0;
	UINT numBuffers = 1;

	context->IASetVertexBuffers(startSlot, numBuffers, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetInputLayout(m_VertexInputLayout.Get());

    if (!flag)
    {
        context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
        context->PSSetShader(m_PixelShader.Get(), nullptr, 0);
        flag = true;
    }
	UINT startIndexLocation = 0;
	UINT baseVertexLocation = 0;
	context->DrawIndexed(m_IndicesCount, startIndexLocation, baseVertexLocation);
}

void GraphicsComponent::SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices)
{
	m_IndicesCount = indices.size();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = m_IndicesCount * sizeof(uint32_t);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(uint32_t);
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = indices.data();

	THROW_IF_FAILED(
		device->CreateBuffer(
			&desc,
			&initData,
			m_IndexBuffer.GetAddressOf()));
}

void GraphicsComponent::SetVertexBuffer(ID3D11Device* device, const std::vector<Vertex>& vertices)
{
	size_t verticesCount = vertices.size();

	D3D11_BUFFER_DESC desc = {};
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = verticesCount * sizeof(Vertex);
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(Vertex);
	desc.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA initData = {};
	initData.pSysMem = vertices.data();

	THROW_IF_FAILED(
		device->CreateBuffer(
			&desc,
			&initData,
			m_VertexBuffer.GetAddressOf()));
}

void GraphicsComponent::SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	context->IASetPrimitiveTopology(topology);
}

void GraphicsComponent::ChangeVertexBufferData(ID3D11DeviceContext* context, const std::vector<Vertex>& vertices)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    THROW_IF_FAILED(
        context->Map(
            m_VertexBuffer.Get(),
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource));

    memcpy(mappedResource.pData, vertices.data(), sizeof(Vertex) * vertices.size());

    context->Unmap(m_VertexBuffer.Get(), 0);
}

void GraphicsComponent::ChangeIndexBufferData(ID3D11DeviceContext* context, const std::vector<uint32_t>& indices)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    THROW_IF_FAILED(
        context->Map(                       // mapping the resource blocks gpu from accessing it
            m_IndexBuffer.Get(),            // the resource
            0,                              // the index of the subresource
            D3D11_MAP_WRITE_DISCARD,        // Resource is mapped for writing; the previous contents of the resource will be undefined
            0,                              // specifies what the CPU does when the GPU is busy (Optional)
            &mappedResource));

    memcpy(mappedResource.pData, indices.data(), sizeof(uint32_t) * indices.size());        // copy the updated data to the mapped subresource

    context->Unmap(m_IndexBuffer.Get(), 0);

}

void GraphicsComponent::InitVertexShader(ID3D11Device* device, const LPCWSTR filePath)
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

void GraphicsComponent::InitPixelShader(ID3D11Device* device, const LPCWSTR filePath)
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

void GraphicsComponent::InitVertexInputLayout(ID3D11Device* device, const LPCWSTR filePath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
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
