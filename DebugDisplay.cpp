#include "DebugDisplay.h"
#include "Vector4f.h"

DebugDisplay* DebugDisplay::ms_DebudDisplay = nullptr;

DebugDisplay::DebugDisplay(ID3D11Device* device) 
{

}

void DebugDisplay::Setup3DBoxBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = 1024 * sizeof(DirectX::XMMATRIX);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(DirectX::XMMATRIX);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxedInstanceBuffer.GetAddressOf()));

    desc.ByteWidth = 8 * sizeof(Vector4f);          // 8 vertices per box
    desc.StructureByteStride = sizeof(Vector4f);
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxedVertexBuffer.GetAddressOf()));
}

void DebugDisplay::Draw3DBox(Vector3<float> pos, Vector3<float> center, Vector3<float> extent)
{
    if (m_3DBoxesCount == MAX_ELEMENT)
        return;

    m_3DBoxes[m_3DBoxesCount] = Debug3DBox{ pos, center, extent };
    ++m_3DBoxesCount;
}

void DebugDisplay::Render(ID3D11DeviceContext* context)
{
    for (const auto& box3D : m_3DBoxes)
    {

    }
}