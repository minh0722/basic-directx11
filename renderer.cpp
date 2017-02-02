#include "pch.h"
#include "renderer.h"
#include "GraphicsComponent.h"

float cos45 = std::cos(PI / 4);
float sin45 = std::sin(PI / 4);

Renderer::Renderer()
{
}

void Renderer::Initialize(HWND window)
{
	m_Window = window;
	InitSwapChain(window);
	InitRenderTargetView(m_SwapChain.Get());
	InitViewPort();
	
	InitDepthStencilBuffer();
	InitDepthStencil();
	InitRasterizerState();
	
	SetupTriangle();
	SetupCube();
}

void Renderer::Render()
{
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//m_Triangle.Render(m_DeviceContext.Get());
	m_Cube.Render(m_DeviceContext.Get());
		
	m_SwapChain->Present(0, 0);
}

void Renderer::InitSwapChain(HWND window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 1080;
	swapChainDesc.BufferDesc.Height = 720;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// the window for the swap chain
	swapChainDesc.OutputWindow = m_Window;

	// turn the multisampling off
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// set to windowed mode
	swapChainDesc.Windowed = true;

	// discard the back buffer contents after presenting
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// don't set the advanced flag
	swapChainDesc.Flags = 0;

	const D3D_FEATURE_LEVEL feature[] = { D3D_FEATURE_LEVEL_11_1 };

	THROW_IF_FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			D3D11_CREATE_DEVICE_SINGLETHREADED,
			feature,
			ARRAYSIZE(feature),
			D3D11_SDK_VERSION,
			&swapChainDesc,
			m_SwapChain.GetAddressOf(),
			m_Device.GetAddressOf(),
			nullptr,
			m_DeviceContext.GetAddressOf()));
}

void Renderer::InitRenderTargetView(IDXGISwapChain * swapChain)
{
	ComPtr<ID3D11Texture2D> backBufferTexture;
	THROW_IF_FAILED(
		swapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(backBufferTexture.GetAddressOf())));

	THROW_IF_FAILED(
		m_Device->CreateRenderTargetView(
			backBufferTexture.Get(),
			nullptr,
			m_RenderTargetView.GetAddressOf()));

	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), nullptr);
}

void Renderer::InitDepthStencilBuffer()
{
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc = {};

	depthStencilBufferDesc.Width = screenWidth;
	depthStencilBufferDesc.Height = screenHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0;
	depthStencilBufferDesc.MiscFlags = 0;

	THROW_IF_FAILED(
		m_Device->CreateTexture2D(
			&depthStencilBufferDesc, 
			nullptr, 
			m_DepthStencilBuffer.GetAddressOf()));
}

void Renderer::InitDepthStencil()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
	depthStencilDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

	// Stencil operations if pixel is front_facing
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	THROW_IF_FAILED(
		m_Device->CreateDepthStencilState(
			&depthStencilDesc,
			m_DepthStencilState.GetAddressOf()));

	//m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);


	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	THROW_IF_FAILED(
		m_Device->CreateDepthStencilView(
			m_DepthStencilBuffer.Get(), 
			&depthStencilViewDesc, 
			m_DepthStencilView.GetAddressOf()));

	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);
}

void Renderer::InitRasterizerState()
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = D3D11_CULL_BACK;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = D3D11_FILL_SOLID;
	desc.FrontCounterClockwise = true;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	THROW_IF_FAILED(m_Device->CreateRasterizerState(&desc, m_RasterizerState.GetAddressOf()));

	m_DeviceContext->RSSetState(m_RasterizerState.Get());
}

void Renderer::InitViewPort()
{
	D3D11_VIEWPORT desc = {};
	desc.TopLeftX = 0.0f;
	desc.TopLeftY = 0.0f;
	desc.Width = screenWidth;
	desc.Height = screenHeight;
	//desc.MinDepth = 0.0f;
	//desc.MaxDepth = 1.0f;

	m_DeviceContext->RSSetViewports(1, &desc);
}

void Renderer::SetupTriangle()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexShaderInputLayout(2);
	vertexShaderInputLayout[0].SemanticName = "POSITION";
	vertexShaderInputLayout[0].SemanticIndex = 0;								// will use POSITION0 semantic
	vertexShaderInputLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
	vertexShaderInputLayout[0].InputSlot = 0;									// 0 ~ 15
	vertexShaderInputLayout[0].AlignedByteOffset = 0;
	vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
	vertexShaderInputLayout[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

	vertexShaderInputLayout[1].SemanticName = "COLOR";
	vertexShaderInputLayout[1].SemanticIndex = 0;
	vertexShaderInputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexShaderInputLayout[1].InputSlot = 0;
	vertexShaderInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexShaderInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexShaderInputLayout[1].InstanceDataStepRate = 0;

	GraphicsComponent::GraphicsComponentDesc desc =
	{
		m_Device.Get(),
		L"vertexShader.cso",
		L"pixelShader.cso",
		vertexShaderInputLayout
	};

	BaseComponent* graphicComponent = new GraphicsComponent(desc);
	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicComponent->SetIndexBuffer(m_Device.Get(), {0, 1, 2});
	graphicComponent->SetVertexBuffer(
		m_Device.Get(),
		{
			//			pos							color
			{ {-1.0f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f} },		// bottom left
			{ { 1.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f} },		// bottom right
			{ { 0.0f,  1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f} }		// top middle
		}
	);

	m_Triangle.AddComponent(graphicComponent);
}

void Renderer::SetupCube()
{
	std::vector<D3D11_INPUT_ELEMENT_DESC> vertexShaderInputLayout(2);
	vertexShaderInputLayout[0].SemanticName = "POSITION";
	vertexShaderInputLayout[0].SemanticIndex = 0;								// will use POSITION0 semantic
	vertexShaderInputLayout[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
	vertexShaderInputLayout[0].InputSlot = 0;									// 0 ~ 15
	vertexShaderInputLayout[0].AlignedByteOffset = 0;
	vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
	vertexShaderInputLayout[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

	vertexShaderInputLayout[1].SemanticName = "COLOR";
	vertexShaderInputLayout[1].SemanticIndex = 0;
	vertexShaderInputLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	vertexShaderInputLayout[1].InputSlot = 0;
	vertexShaderInputLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	vertexShaderInputLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	vertexShaderInputLayout[1].InstanceDataStepRate = 0;

	GraphicsComponent::GraphicsComponentDesc desc =
	{
		m_Device.Get(),
		L"vertexShader.cso",
		L"pixelShader.cso",
		vertexShaderInputLayout
	};

    Color color = { 1.0f, 0.0f, 0.0f, 0.0f };
    
    // left handed coordinate system. Same as directx
    std::vector<Vertex> vertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, color },
        { { 1.0f, 0.0f, 0.0f, 1.0f }, color },
        { { 1.0f, 0.0f, 1.0f, 1.0f }, color },
        { { 0.0f, 0.0f, 1.0f, 1.0f }, color },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, color },
        { { 1.0f, 1.0f, 0.0f, 1.0f }, color },
        { { 1.0f, 1.0f, 1.0f, 1.0f }, color },
        { { 0.0f, 1.0f, 1.0f, 1.0f }, color }
    };

    // do transform
    Matrix44f translation =
    {
        1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 2.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };

    // rotation 45 degrees around y axis
    Matrix44f rotation =
    {
        cos45, 0.0f, sin45, 0.0f,
        0.0f,  1.0f, 0.0f,  0.0f,
       -sin45, 0.0f, cos45, 0.0f,
        0.0f,  0.0f, 0.0f,  1.0f
    };

    // rotate then translate
    Matrix44f transformMatrix = translation * rotation;

    // after this coordinates are in world
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        vertices[i].pos = transformMatrix * vertices[i].pos;
    }


	BaseComponent* graphicComponent = new GraphicsComponent(desc);
	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
	graphicComponent->SetIndexBuffer(m_Device.Get(), { 0, 2, 1 });
	graphicComponent->SetVertexBuffer(
		m_Device.Get(),
		{
			//			pos							color
			{ { -1.0f, -1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 0.0f } },	// bottom left
			{ {  1.0f, -1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 0.0f } },	// bottom right
			{ {  0.0f,  1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f, 0.0f, 0.0f } },	// top middle
		}
	);

	//TODO: do tranformation of cude from local coordinates

	m_Cube.AddComponent(graphicComponent);
}
