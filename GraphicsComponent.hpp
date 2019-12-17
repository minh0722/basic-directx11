#pragma once

template <typename VertexBufferType>
void GraphicsComponent::SetVertexBuffer(ID3D11Device* device, const std::vector<VertexBufferType>& vertices)
{
    m_VerticesCount = (UINT)vertices.size();
    m_VertexBufferStride = sizeof(VertexBufferType);
    UINT verticesCount = (UINT)vertices.size();

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = verticesCount * sizeof(VertexBufferType);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(VertexBufferType);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    THROW_IF_FAILED(
        device->CreateBuffer(
            &desc,
            &initData,
            m_VertexBuffer.GetAddressOf()));
}

template <typename VertexBufferType>
void GraphicsComponent::AddVertexBatch(ID3D11Device* device, const std::vector<VertexBufferType>& vertices)
{
    UINT verticesCount = (UINT)vertices.size();
    UINT vertexBufferStride = sizeof(VertexBufferType);

    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = verticesCount * sizeof(VertexBufferType);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(VertexBufferType);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices.data();

    Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;

    THROW_IF_FAILED(
        device->CreateBuffer(
            &desc,
            &initData,
            buffer.GetAddressOf()));

    m_vertexBufferBatches.push_back({buffer, verticesCount, vertexBufferStride});
}