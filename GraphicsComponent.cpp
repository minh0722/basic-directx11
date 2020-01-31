#include "pch.h"
#include "GraphicsComponent.h"
#include "renderer.h"
#include "WICTextureLoader.h"
#include "DebugDisplay.h"
#include "ImpostorBaker.h"
#include "ImpostorRenderer.h"

GraphicsComponent::GraphicsComponent(const GraphicsComponentDesc& desc)
{
	InitVertexShader(desc.device, desc.vertexShaderFilePath);
	InitPixelShader(desc.device, desc.pixelShaderFilePath);
	InitVertexInputLayout(desc.device, desc.vertexShaderFilePath, desc.vertexInputLayout);
    InitWorldViewProjBuffer(desc.device);

	// TODO: handle exceptions here...
}

void GraphicsComponent::Render(Renderer* renderer, bool isInstanceRendering /*= false*/, uint32_t instanceCount /*= 1*/)
{
    if (m_ImpostorAlbedoAtlasSRV && m_ImpostorNormalAtlasSRV)
    {
        ImpostorRenderer::Render(renderer, this);
        return;
    }

    ID3D11DeviceContext* context = renderer->GetContext();
    context->VSSetShader(m_VertexShader.Get(), nullptr, 0);
    context->PSSetShader(m_PixelShader.Get(), nullptr, 0);

    if (m_TextureSRV)
        context->PSSetShaderResources(0, 1, m_TextureSRV.GetAddressOf());
    
    UINT stride = m_VertexBufferStride;
	UINT offset = 0;
	UINT startSlot = 0;
	UINT numBuffers = 1;

    if (m_Batches.size() > 0)
    {
        for (auto it = m_Batches.begin(); it != m_Batches.end(); ++it)
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

void GraphicsComponent::BakeImpostor(ID3D11Device* device, ID3D11DeviceContext* context)
{
    if (m_Batches.size() > 0)
    {
        UINT offset = 0;
        UINT startSlot = 0;
        UINT numBuffers = 1;

        if (m_TextureSRV)
            context->PSSetShaderResources(0, 1, m_TextureSRV.GetAddressOf());

        BakeResult result = ImpostorBaker::Bake(context, this);

        m_ImpostorAlbedoAtlasTexture.Attach(result.m_AlbedoBakedTexture);
        m_ImpostorNormalAtlasTexture.Attach(result.m_NormalDepthTexture);

        D3D11_TEXTURE2D_DESC desc = {};
        m_ImpostorAlbedoAtlasTexture->GetDesc(&desc);

        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        srvDesc.Texture2D.MostDetailedMip = 0;

        THROW_IF_FAILED(device->CreateShaderResourceView(m_ImpostorAlbedoAtlasTexture.Get(), &srvDesc, m_ImpostorAlbedoAtlasSRV.GetAddressOf()));

        m_ImpostorNormalAtlasTexture->GetDesc(&desc);
        srvDesc.Format = desc.Format;
        srvDesc.Texture2D.MipLevels = desc.MipLevels;
        THROW_IF_FAILED(device->CreateShaderResourceView(m_ImpostorNormalAtlasTexture.Get(), &srvDesc, m_ImpostorNormalAtlasSRV.GetAddressOf()));
    }
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
        DirectX::CreateWICTextureFromFile(device, texturePath, nullptr, m_TextureSRV.ReleaseAndGetAddressOf())
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

const wavefront::AABB& GraphicsComponent::GetBoundingBox() const
{
    return m_BoundingBox;
}

const Microsoft::WRL::ComPtr<ID3D11InputLayout>& GraphicsComponent::GetInputLayout() const
{
    return m_VertexInputLayout;
}

const std::unordered_map<uint32_t, Batch>& GraphicsComponent::GetBatches() const
{
    return m_Batches;
}

const std::unordered_map<uint32_t, Microsoft::WRL::ComPtr<ID3D11Buffer>>& GraphicsComponent::GetMaterialBuffers() const
{
    return m_MaterialBuffers;
}

const Microsoft::WRL::ComPtr<ID3D11Buffer>& GraphicsComponent::GetWorldViewProjBuffer() const
{
    return m_WorldViewProjBuffer;
}

Vector4f GraphicsComponent::GetWorldPos() const
{
    return m_WorldPosition;
}

const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GraphicsComponent::GetImpostorAlbedoSRV() const
{
    return m_ImpostorAlbedoAtlasSRV;
}

const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& GraphicsComponent::GetImpostorNormalDepthSRV() const
{
    return m_ImpostorNormalAtlasSRV;
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
