#include "DebugDisplay.h"
#include "Vector4f.h"
#include "renderer.h"

DebugDisplay* DebugDisplay::ms_DebudDisplay = nullptr;
bool DebugDisplay::ms_EnableDebugDisplay = true;

DebugDisplay::DebugDisplay(ID3D11Device* device, ID3D11DeviceContext* context)
{
    Setup3DBoxesRenderState(device);
    Setup3DBoxBuffers(device);
    SetupViewProjectionBuffer(device);
}

void DebugDisplay::SetupViewProjectionBuffer(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc = {
        2 * sizeof(DirectX::XMMATRIX), 
        D3D11_USAGE_DYNAMIC, 
        D3D11_BIND_CONSTANT_BUFFER, 
        D3D11_CPU_ACCESS_WRITE, 
        0, 
        0};

    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_CameraViewProjectionBuffer.GetAddressOf()));
}

void DebugDisplay::UpdateViewProjectionBuffer(Renderer* renderer)
{
    ID3D11DeviceContext* context = renderer->GetContext();
    const Camera& camera = renderer->GetCamera();
    
    DirectX::XMMATRIX viewProj[2];
    viewProj[0] = camera.GetViewMatrix();
    viewProj[1] = camera.GetProjectionMatrix();

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    THROW_IF_FAILED(context->Map(m_CameraViewProjectionBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

    memcpy(mappedResource.pData, viewProj, 2 * sizeof(DirectX::XMMATRIX));

    context->Unmap(m_CameraViewProjectionBuffer.Get(), 0);
}

void DebugDisplay::Setup3DBoxesRenderState(ID3D11Device* device)
{
    ID3DBlob* blob;
    THROW_IF_FAILED(D3DReadFileToBlob(L"debugdisplay3dbox.cso", &blob));

    THROW_IF_FAILED(
        device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_3DBoxVS.ReleaseAndGetAddressOf())
    );

    THROW_IF_FAILED(D3DReadFileToBlob(L"pixelShader.cso", &blob));
    THROW_IF_FAILED(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, m_3DBoxPS.ReleaseAndGetAddressOf()));
}

void DebugDisplay::Setup3DBoxBuffers(ID3D11Device* device)
{
    D3D11_BUFFER_DESC desc = {};
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_VERTEX_BUFFER;
    desc.ByteWidth = MAX_ELEMENT * sizeof(DirectX::XMMATRIX);
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    desc.StructureByteStride = sizeof(DirectX::XMMATRIX);
    desc.Usage = D3D11_USAGE_DYNAMIC;

    // instance buffer
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxesInstanceBuffer.GetAddressOf()));

    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R32_TYPELESS;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;

    srvDesc.BufferEx.FirstElement = 0;
    srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
    srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
    THROW_IF_FAILED(device->CreateShaderResourceView(m_3DBoxesInstanceBuffer.Get(), &srvDesc, m_3DBoxesInstanceSRV.GetAddressOf()));

    // vertex buffer
    desc.ByteWidth = MAX_ELEMENT * 2 * 12 * sizeof(Vector4f);          // line list with 12 lines * 2 vertices (since they get repeated)
    desc.StructureByteStride = sizeof(Vector4f);
    THROW_IF_FAILED(device->CreateBuffer(&desc, nullptr, m_3DBoxesVertexBuffer.GetAddressOf()));

    srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
    THROW_IF_FAILED(device->CreateShaderResourceView(m_3DBoxesVertexBuffer.Get(), &srvDesc, m_3DBoxesVertexSRV.GetAddressOf()));

}

void DebugDisplay::Update3DBoxBuffers(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource = {};

    THROW_IF_FAILED(context->Map(m_3DBoxesVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

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
		
		Vector4f a = Vector4f(box3D.m_center + box3D.m_extent, 1.0f);
		Vector4f b = Vector4f(a.XYZ() - Vector3<float>(2.0f * box3D.m_extent.x, 0.0f, 0.0f), 1.0f);
		Vector4f c = Vector4f(b.XYZ() - Vector3<float>(0.0f, 0.0f, 2.0f * box3D.m_extent.z), 1.0f);
		Vector4f d = Vector4f(a.XYZ() - Vector3<float>(0.0f, 0.0f, 2.0f * box3D.m_extent.z), 1.0f);
		Vector4f e = Vector4f(a.XYZ() - Vector3<float>(0.0f, 2.0f * box3D.m_extent.y, 0.0f), 1.0f);
		Vector4f f = Vector4f(e.XYZ() - Vector3<float>(2.0f * box3D.m_extent.x, 0.0f, 0.0f), 1.0f);
		Vector4f g = Vector4f(f.XYZ() - Vector3<float>(0.0f, 0.0f, 2.0f * box3D.m_extent.z), 1.0f);
		Vector4f h = Vector4f(e.XYZ() - Vector3<float>(0.0f, 0.0f, 2.0f * box3D.m_extent.z), 1.0f);

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

    context->Unmap(m_3DBoxesVertexBuffer.Get(), 0);

    THROW_IF_FAILED(context->Map(m_3DBoxesInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource));

    for (uint32_t i = 0; i < m_3DBoxesCount; ++i)
    {
        const Debug3DBox& box = m_3DBoxes[i];
        
        DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(box.m_pos.x, box.m_pos.y, box.m_pos.z);

        memcpy(mappedResource.pData, &worldMatrix, sizeof(DirectX::XMMATRIX));
        mappedResource.pData = (uint8_t*)mappedResource.pData + sizeof(DirectX::XMMATRIX);
    }
    context->Unmap(m_3DBoxesInstanceBuffer.Get(), 0);
}

void DebugDisplay::Draw3DBox(Vector3<float> pos, Vector3<float> center, Vector3<float> extent)
{
    if (m_3DBoxesCount == MAX_ELEMENT)
        return;

    m_3DBoxes[m_3DBoxesCount] = Debug3DBox{ pos, center, extent };
    ++m_3DBoxesCount;
}

void DebugDisplay::Render(Renderer* renderer)
{
    if (!ms_EnableDebugDisplay)
        return;

    ID3D11DeviceContext* context = renderer->GetContext();

    UpdateViewProjectionBuffer(renderer);
    Update3DBoxBuffers(renderer->GetContext());

    renderer->SetRasterizerState(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE);
    context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
    context->VSSetShader(m_3DBoxVS.Get(), nullptr, 0);
    context->PSSetShader(m_3DBoxPS.Get(), nullptr, 0);

    context->VSSetShaderResources(0, 1, m_3DBoxesVertexSRV.GetAddressOf());
    context->VSSetShaderResources(1, 1, m_3DBoxesInstanceSRV.GetAddressOf());
    context->VSSetConstantBuffers(0, 1, m_CameraViewProjectionBuffer.GetAddressOf());
    context->IASetInputLayout(nullptr);

    context->DrawInstanced(12 * 2, m_3DBoxesCount, 0, 0);
}

void DebugDisplay::OnNewFrame()
{
    m_3DBoxesCount = 0;
}

void DebugDisplay::ToggleDebugDisplay()
{
    ms_EnableDebugDisplay = !ms_EnableDebugDisplay;
}