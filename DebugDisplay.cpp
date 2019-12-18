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
    desc.ByteWidth = MAX_ELEMENT * sizeof(DirectX::XMMATRIX);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;
    desc.StructureByteStride = sizeof(DirectX::XMMATRIX);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxedInstanceBuffer.GetAddressOf()));

    desc.ByteWidth = MAX_ELEMENT * 2 * 12 * sizeof(Vector4f);          // line list with 12 lines * 2 vertices (since they get repeated)
    desc.StructureByteStride = sizeof(Vector4f);
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxedVertexBuffer.GetAddressOf()));
}

void DebugDisplay::Change3DBoxBuffers(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    THROW_IF_FAILED(
        context->Map(
            m_3DBoxedVertexBuffer.Get(),
            0,
            D3D11_MAP_WRITE_DISCARD,
            0,
            &mappedResource));

	/*
		  b	_________ a
		   /|       /|
		 c/_|______/ |
		  |	|	   |d|
		  |f|______|_|
		  |	/	   | /e
		  |/_______|/
		  g			h
	*/

	for (uint32_t i = 0; i < m_3DBoxesCount; ++i)
	{
		const auto& box3D = m_3DBoxes[i];
		Vector4f buffer[12 * 2];
		
		Vector4f a = Vector4f(box3D.m_center + box3D.m_extent, 0.0f);
		Vector4f b = a - Vector4f(2.0f * box3D.m_extent.x, 0.0f, 0.0f, 0.0f);
		Vector4f c = b - Vector4f(0.0f, 0.0f, 2.0f * box3D.m_extent.z, 0.0f);
		Vector4f d = a - Vector4f(0.0f, 0.0f, 2.0f * box3D.m_extent.z, 0.0f);
		Vector4f e = a - Vector4f(0.0f, 2.0f * box3D.m_extent.y, 0.0f, 0.0f);
		Vector4f f = e - Vector4f(2.0f * box3D.m_extent.x, 0.0f, 0.0f, 0.0f);
		Vector4f g = f - Vector4f(0.0f, 0.0f, 2.0f * box3D.m_extent.z, 0.0f);
		Vector4f h = e - Vector4f(0.0f, 0.0f, 2.0f * box3D.m_extent.z, 0.0f);

		int32_t j = -1;
		buffer[++j] = a; buffer[++j] = b;
		buffer[++j] = b; buffer[++j] = c;
		buffer[++j] = c; buffer[++j] = d;
		buffer[++j] = d; buffer[++j] = a;

		buffer[++j] = e; buffer[++j] = f;
		buffer[++j] = f; buffer[++j] = g;
		buffer[++j] = g; buffer[++j] = h;
		buffer[++j] = h; buffer[++j] = e;
		
		buffer[++j] = e; buffer[++j] = a;
		buffer[++j] = h; buffer[++j] = d;
		buffer[++j] = f; buffer[++j] = b;
		buffer[++j] = g; buffer[++j] = c;

		memcpy(mappedResource.pData, buffer, sizeof(Vector4f) * 12 * 2);

		mappedResource.pData = (uint8_t*)mappedResource.pData + sizeof(Vector4f) * 12 * 2;
	}

    context->Unmap(m_3DBoxedVertexBuffer.Get(), 0);
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