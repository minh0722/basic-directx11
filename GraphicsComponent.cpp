#include "pch.h"
#include "GraphicsComponent.h"
#include "renderer.h"
#include "WICTextureLoader.h"
#include "DebugDisplay.h"

GraphicsComponent::GraphicsComponent(const GraphicsComponentDesc& desc)
{
	InitVertexShader(desc.device, desc.vertexShaderFilePath);
	InitPixelShader(desc.device, desc.pixelShaderFilePath);
	InitVertexInputLayout(desc.device, desc.vertexShaderFilePath, desc.vertexInputLayout);
    InitWorldViewProjBuffer(desc.device);

	// TODO: handle exceptions here...
}

void GraphicsComponent::Render(ID3D11DeviceContext* context, bool isInstanceRendering /*= false*/, uint32_t instanceCount /*= 1*/)
{
    context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

    if (m_Texture)
        context->PSSetShaderResources(0, 1, m_Texture.GetAddressOf());
    
    UINT stride = m_VertexBufferStride;
	UINT offset = 0;
	UINT startSlot = 0;
	UINT numBuffers = 1;

    if (m_VertexBatches.size() > 0)
    {
        for (auto it = m_VertexBatches.begin(); it != m_VertexBatches.end(); ++it)
        {
            const uint32_t materialID = it->first;
            const Batch& batch = it->second;

            context->IASetVertexBuffers(startSlot, numBuffers, batch.vertexBuffer.GetAddressOf(), &batch.vertexBufferStride, &offset);
            context->IASetInputLayout(m_VertexInputLayout.Get());
            context->VSSetConstantBuffers(0, 1, m_WorldViewProjBuffer.GetAddressOf());
            context->PSSetConstantBuffers(0, 1, m_MaterialBuffers[materialID].GetAddressOf());

            if (isInstanceRendering)
            {
                context->DrawIndexedInstanced(
                    m_IndicesCount,         // Number of indices read from the index buffer for each instance. 
                    instanceCount,          // Number of instances to draw
                    0,                      // The location of the first index read by the GPU from the index buffer
                    0,                      // A value added to each index before reading a vertex from the vertex buffer
                    0);                     // A value added to each index before reading per-instance data from a vertex buffer
            }
            else
            {
                if (m_drawType == wavefront::DrawType::Draw)
                    context->Draw(batch.verticesCount, 0);
                else
                    context->DrawIndexed(m_IndicesCount, 0, 0);
            }
        }
    }
    else
    {
	    context->IASetVertexBuffers(startSlot, numBuffers, m_VertexBuffer.GetAddressOf(), &stride, &offset);
	    context->IASetIndexBuffer(m_IndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	    context->IASetInputLayout(m_VertexInputLayout.Get());
        context->VSSetConstantBuffers(0, 1, m_WorldViewProjBuffer.GetAddressOf());
	
        if (isInstanceRendering)
        {
            context->DrawIndexedInstanced(
                m_IndicesCount,         // Number of indices read from the index buffer for each instance. 
                instanceCount,          // Number of instances to draw
                0,                      // The location of the first index read by the GPU from the index buffer
                0,                      // A value added to each index before reading a vertex from the vertex buffer
                0);                     // A value added to each index before reading per-instance data from a vertex buffer
        }
        else
        {
            if (m_drawType == wavefront::DrawType::Draw)
                context->Draw(m_VerticesCount, 0);
            else
                context->DrawIndexed(m_IndicesCount, 0, 0);
        }
    }

    if(m_BoundingBox.m_halfVec != Vector4f(0.0f, 0.0f, 0.0f, 0.0f))
        DebugDisplay::GetDebugDisplay().Draw3DBox(m_WorldPosition.XYZ(), m_BoundingBox.m_center.XYZ(), m_BoundingBox.m_halfVec.XYZ());
}

void GraphicsComponent::BakeImpostor()
{

}

void GraphicsComponent::SetIndexBuffer(ID3D11Device* device, const std::vector<uint32_t>& indices)
{
    m_IndicesCount = (UINT)indices.size();

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

void GraphicsComponent::SetIndexBuffer(ID3D11Device* device, const void* indices, size_t indicesCount)
{
    m_IndicesCount = (UINT)indicesCount;

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    desc.ByteWidth = m_IndicesCount * sizeof(uint32_t);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(uint32_t);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = indices;

    THROW_IF_FAILED(
        device->CreateBuffer(
            &desc,
            &initData,
            m_IndexBuffer.GetAddressOf()));
}

void GraphicsComponent::InitWorldViewProjBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = sizeof(WorldViewProj);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    THROW_IF_FAILED(
        device->CreateBuffer(
            &desc,
            nullptr,
            m_WorldViewProjBuffer.GetAddressOf()));
}

void GraphicsComponent::SetPrimitiveTopology(ID3D11DeviceContext* context, D3D11_PRIMITIVE_TOPOLOGY topology)
{
	context->IASetPrimitiveTopology(topology);
}

void GraphicsComponent::SetSamplerState(ID3D11DeviceContext* context)
{
    if(m_SamplerState)
        context->PSSetSamplers(0, 1, m_SamplerState.GetAddressOf());
}

void GraphicsComponent::SetDrawType(wavefront::DrawType drawType)
{
    m_drawType = drawType;
}

void GraphicsComponent::SetBoundingBox(const wavefront::AABB& boundingBox)
{
    m_BoundingBox = boundingBox;
}

void GraphicsComponent::SetWorldPosition(Vector4f pos)
{
    m_WorldPosition = pos;
}

void GraphicsComponent::LoadTexture(ID3D11Device* device, const wchar_t* texturePath)
{
    THROW_IF_FAILED(
        DirectX::CreateWICTextureFromFile(device, texturePath, nullptr, m_Texture.ReleaseAndGetAddressOf())
    );
}

void GraphicsComponent::AddMaterial(ID3D11Device* device, uint32_t materialID, wavefront::Material material)
{
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.ByteWidth = sizeof(wavefront::Material);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = 0;
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = &material;

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    THROW_IF_FAILED(device->CreateBuffer(&desc, &initData, buffer.GetAddressOf()));

    m_MaterialBuffers[materialID] = buffer;
}

//void GraphicsComponent::ChangeVertexBufferData(ID3D11DeviceContext* context, const std::vector<Vertex>& vertices)
//{
//    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
//
//    THROW_IF_FAILED(
//        context->Map(
//            m_VertexBuffer.Get(),
//            0,
//            D3D11_MAP_WRITE_DISCARD,
//            0,
//            &mappedResource));
//
//    memcpy(mappedResource.pData, vertices.data(), sizeof(Vertex) * vertices.size());
//
//    context->Unmap(m_VertexBuffer.Get(), 0);
//}

//void GraphicsComponent::ChangeIndexBufferData(ID3D11DeviceContext* context, const std::vector<uint32_t>& indices)
//{
//    D3D11_MAPPED_SUBRESOURCE mappedResource = {};
//
//    THROW_IF_FAILED(
//        context->Map(                       // mapping the resource blocks gpu from accessing it
//            m_IndexBuffer.Get(),            // the resource
//            0,                              // the index of the subresource
//            D3D11_MAP_WRITE_DISCARD,        // Resource is mapped for writing; the previous contents of the resource will be undefined
//            0,                              // specifies what the CPU does when the GPU is busy (Optional)
//            &mappedResource));
//
//    memcpy(mappedResource.pData, indices.data(), sizeof(uint32_t) * indices.size());        // copy the updated data to the mapped subresource
//
//    context->Unmap(m_IndexBuffer.Get(), 0);
//
//}

void GraphicsComponent::ChangeWorldViewProjBufferData(ID3D11DeviceContext* context, const WorldViewProj& worldViewProj)
{
    D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
    
    THROW_IF_FAILED(
        context->Map(
            m_WorldViewProjBuffer.Get(),
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedSubresource));

    memcpy(mappedSubresource.pData, &worldViewProj, sizeof(WorldViewProj));

    context->Unmap(m_WorldViewProjBuffer.Get(), 0);
}

void GraphicsComponent::InitSamplerState(ID3D11Device* device, D3D11_SAMPLER_DESC desc)
{
    THROW_IF_FAILED(
        device->CreateSamplerState(&desc, m_SamplerState.GetAddressOf())
    );
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
			m_VertexShader.ReleaseAndGetAddressOf()));
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
			m_PixelShader.ReleaseAndGetAddressOf()));
}

void GraphicsComponent::InitVertexInputLayout(ID3D11Device* device, const LPCWSTR filePath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& inputLayoutDesc)
{
	ID3DBlob* vertexBlob;
	THROW_IF_FAILED(D3DReadFileToBlob(filePath, &vertexBlob));

	THROW_IF_FAILED(
		device->CreateInputLayout(
			inputLayoutDesc.data(),
			(UINT)inputLayoutDesc.size(),
			vertexBlob->GetBufferPointer(),
			vertexBlob->GetBufferSize(),
			m_VertexInputLayout.GetAddressOf()));
}
