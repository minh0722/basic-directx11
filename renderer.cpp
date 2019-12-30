#include "pch.h"
#include "renderer.h"
#include "GraphicsComponent.h"
#include "Matrix44f.h"
#include "inputclass.h"
#include "ObjLoader.h"
#include "Octahedron.h"
#include "Hemioctahedron.h"
#include "DebugDisplay.h"
#include "ImpostorBaker.h"
#include <cmath>

float cos45 = (float)std::cos(PI / 4);
float sin45 = (float)std::sin(PI / 4);

Renderer* Renderer::ms_Instance = nullptr;

Renderer::Renderer()
	: m_Camera(
		DirectX::XMVectorSet(0.0f, 3.0f, 0.0f, 1.0f),		// camera position
		60.0f)												// fov
{
}

Renderer::~Renderer()
{
    delete m_DebugDisplay;
}

void Renderer::Initialize(HWND window)
{
	m_Window = window;
    InitDeviceSwapChainAndDeviceContext(window);
    InitDepthStencilBufferAndView();
    InitDepthStencilState();
	InitRenderTargetView(m_SwapChain.Get());
		
	SetupTriangle();
	SetupCube();
    SetupAxis();

	SetupSphereMesh();
    SetupOctahedronMesh();
    SetupHemioctahedronMesh();

	SetupSpaceShip();

    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    long long f = freq.QuadPart;

    m_DebugDisplay = new DebugDisplay(m_Device.Get(), m_DeviceContext.Get());
    DebugDisplay::SetDebugDisplay(m_DebugDisplay);

	ImpostorBaker::Initialize(this);
}

void Renderer::Render(InputClass* input)
{
    bool hasInput = onInput(input, m_Camera);
    m_DebugDisplay->OnNewFrame();
    m_Camera.UpdateCameraMatrices();

	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
	m_DeviceContext->OMSetRenderTargets(1, m_RenderTargetView.GetAddressOf(), m_DepthStencilView.Get());
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState.Get(), 1);
	SetViewPort();

	SetRasterizerState(D3D11_FILL_SOLID);
	//m_Triangle.Render(m_DeviceContext.Get());
    SetupPrimitiveForRender(hasInput);
	m_Cube.Render(m_DeviceContext.Get(), true, 10000);
	SetupPrimitiveForRender(hasInput, Line);
    m_Axis.Render(m_DeviceContext.Get());
	
	SetRasterizerState(D3D11_FILL_WIREFRAME, D3D11_CULL_FRONT);
	SetupPrimitiveForRender(hasInput, Sphere);
	m_SphereMesh.Render(m_DeviceContext.Get());

    SetRasterizerState(D3D11_FILL_WIREFRAME, D3D11_CULL_BACK);
    SetupPrimitiveForRender(hasInput, Octahedral);
    m_OctahedronMesh.Render(m_DeviceContext.Get());
    
    SetRasterizerState(D3D11_FILL_WIREFRAME, D3D11_CULL_NONE);
    SetupPrimitiveForRender(hasInput, Hemioctahedral);
    m_HemioctahedronMesh.Render(m_DeviceContext.Get());

	// TODO: spaceship vertex buffer input layout doesnt use color so create a new shader or find a way 
	// to set define in the shader to not use color when we are rendering the spaceship
	SetRasterizerState(D3D11_FILL_SOLID, D3D11_CULL_NONE);
	SetupSpaceShipForRender(hasInput);
	m_SpaceShip.Render(m_DeviceContext.Get());

    m_DebugDisplay->Render(this);

	//m_SpaceShip.BakeImpostor(m_DeviceContext.Get());

	m_SwapChain->Present(0, 0);
}

ID3D11Device* Renderer::GetDevice()
{
    return m_Device.Get();
}

ID3D11DeviceContext* Renderer::GetContext()
{
    return m_DeviceContext.Get();
}

const Camera& Renderer::GetCamera()
{
    return m_Camera;
}

void Renderer::InitDeviceSwapChainAndDeviceContext(HWND window)
{
	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
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

	const D3D_FEATURE_LEVEL feature[] = { 
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0
	};

	HRESULT hr = E_FAIL;
	UINT flags = D3D11_CREATE_DEVICE_DEBUG;
	while (hr != S_OK)
	{
		hr = D3D11CreateDeviceAndSwapChain(
				nullptr,
				D3D_DRIVER_TYPE_HARDWARE,
				nullptr,
				flags,
				feature,
				ARRAYSIZE(feature),
				D3D11_SDK_VERSION,
				&swapChainDesc,
				m_SwapChain.GetAddressOf(),
				m_Device.GetAddressOf(),
				nullptr,
				m_DeviceContext.GetAddressOf());

		if (hr != S_OK)
			flags = 0;
	}
}

void Renderer::InitRenderTargetView(IDXGISwapChain * swapChain)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
	THROW_IF_FAILED(
		swapChain->GetBuffer(
			0,
			__uuidof(ID3D11Texture2D),
			reinterpret_cast<LPVOID*>(backBufferTexture.GetAddressOf())));

	THROW_IF_FAILED(
		m_Device->CreateRenderTargetView(
			backBufferTexture.Get(),
			nullptr,
			m_RenderTargetView.ReleaseAndGetAddressOf()));
}

void Renderer::InitDepthStencilBufferAndView()
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
			m_DepthStencilBuffer.ReleaseAndGetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    THROW_IF_FAILED(
        m_Device->CreateDepthStencilView(
            m_DepthStencilBuffer.Get(),
            &depthStencilViewDesc,
            m_DepthStencilView.ReleaseAndGetAddressOf()));
}

void Renderer::InitDepthStencilState()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
    depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = false;//true;
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
			m_DepthStencilState.ReleaseAndGetAddressOf()));
}

void Renderer::SetRasterizerState(D3D11_FILL_MODE mode /* = D3D11_FILL_SOLID*/, D3D11_CULL_MODE cullMode /*= D3D11_CULL_BACK*/)
{
	D3D11_RASTERIZER_DESC desc = {};
	desc.AntialiasedLineEnable = false;
	desc.CullMode = cullMode;
	desc.DepthBias = 0;
	desc.DepthBiasClamp = 0.0f;
	desc.DepthClipEnable = true;
	desc.FillMode = mode;
	desc.FrontCounterClockwise = true;
	desc.MultisampleEnable = false;
	desc.ScissorEnable = false;
	desc.SlopeScaledDepthBias = 0.0f;

	THROW_IF_FAILED(m_Device->CreateRasterizerState(&desc, m_RasterizerState.ReleaseAndGetAddressOf()));

	m_DeviceContext->RSSetState(m_RasterizerState.Get());
}

void Renderer::SetViewPort()
{
	D3D11_VIEWPORT desc = {};
	desc.TopLeftX = 0.0f;
	desc.TopLeftY = 0.0f;
	desc.Width = screenWidth;
	desc.Height = screenHeight;
	desc.MinDepth = 0.0f;
	desc.MaxDepth = 1.0f;

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
	vertexShaderInputLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each geometry)
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
		std::move(vertexShaderInputLayout)
	};

    std::vector<std::pair<Vector4f, Vector4f>> vertexBuffer = 
    {
        //			pos							color
        { {-1.0f, -1.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f} },		// bottom left
        { { 1.0f, -1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f} },		// bottom right
        { { 0.0f,  1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f} }		// top middle
    };

	GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	graphicComponent->SetIndexBuffer(m_Device.Get(), {0, 1, 2});
	graphicComponent->SetVertexBuffer(m_Device.Get(), vertexBuffer);

	m_Triangle.SetGraphicsComponent(graphicComponent);
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
		L"CubeInstancing_Vs.cso",
		L"pixelShader.cso",
		std::move(vertexShaderInputLayout)
	};

	///////////////////////////////////////////////////////////////////////
    Vector4f red = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vector4f green = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vector4f blue = { 0.0f, 0.0f, 1.0f, 0.0f };

    // left handed coordinate system. Same as directx
    std::vector<Vertex> vertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, blue },
        { { 1.0f, 0.0f, 0.0f, 1.0f }, red },
        { { 1.0f, 0.0f, 1.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 1.0f, 1.0f }, red },
        { { 0.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 1.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 1.0f, 1.0f, 1.0f, 1.0f }, blue },
        { { 0.0f, 1.0f, 1.0f, 1.0f }, green }
    };
    
    Vector4f worldPos(1.0f, 0.0f, 2.0f, 0.0f);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
        
	GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphicComponent->SetIndexBuffer(
        m_Device.Get(),
        { 0, 1, 4, 1, 5, 4,
        1, 2, 5, 5, 2, 6,
        7, 4, 5, 7, 5, 6,
        3, 1, 0, 3, 2, 1,
        7, 6, 2, 7, 2, 3,
        7, 3, 4, 4, 3, 0 });

    graphicComponent->SetVertexBuffer(
        m_Device.Get(),
        vertices);
    
    wavefront::AABB boundingBox;
    boundingBox.Set(Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(1.0f, 1.0f, 1.0f));

    graphicComponent->SetBoundingBox(boundingBox);
    graphicComponent->SetWorldPosition(worldPos);

    graphicComponent->ChangeWorldViewProjBufferData(
        m_DeviceContext.Get(),
		{worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix()});
	
	m_Cube.SetGraphicsComponent(graphicComponent);
}

void Renderer::SetupAxis()
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
        std::move(vertexShaderInputLayout)
    };
	    
    Vector4f red = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vector4f green = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vector4f blue = { 0.0f, 0.0f, 1.0f, 0.0f };

    // left handed coordinate system. Same as directx
    std::vector<Vertex> vertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, red },        // x
        { { 5.0f, 0.0f, 0.0f, 1.0f }, red },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, green },      // y
        { { 0.0f, 5.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, blue },       // z
        { { 0.0f, 0.0f, 5.0f, 1.0f }, blue }
    };

    Vector4f worldPos(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);
	
    GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    graphicComponent->SetIndexBuffer(
        m_Device.Get(),
        { 0, 1, 2, 3, 4, 5 });

    graphicComponent->SetVertexBuffer(
        m_Device.Get(),
        vertices
    );

    wavefront::AABB boundingBox;
    boundingBox.Set(Vector3<float>(0.0f, 0.0f, 0.0f), Vector3<float>(5.0f, 5.0f, 5.0f));
    graphicComponent->SetBoundingBox(boundingBox);
    graphicComponent->SetWorldPosition(worldPos);

	graphicComponent->ChangeWorldViewProjBufferData(
		m_DeviceContext.Get(),
		{worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix()});

    m_Axis.SetGraphicsComponent(graphicComponent);
}

void Renderer::SetupSphereMesh()
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
		std::move(vertexShaderInputLayout)
	};

	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	// calculate vertices for the sphere mesh
	float radius = 7.0f;
	Vector4f green = { 0.0f, 1.0f, 0.0f, 1.0f };
	int latitudeCount = 7;
	int longtitudeCount = 9;

	for (int i = 0; i < latitudeCount; ++i)
	{
		float theta1 = (float)i / (float)latitudeCount * PI;
		float theta2 = (float)((i + 1)) / (float)latitudeCount * PI;

		float sinTheta1 = std::sinf(theta1);
		float sinTheta2 = std::sinf(theta2);
		float cosTheta1 = std::cosf(theta1);
		float cosTheta2 = std::cosf(theta2);

		for (int j = 0; j < longtitudeCount; ++j)
		{
			float phi1 = (float)j / (float)longtitudeCount * 2.0f * PI;			// azimuth goes around 0...2*PI
			float phi2 = (float)((j + 1) % longtitudeCount) / (float)longtitudeCount * 2.0f * PI;

			float cosPhi1 = std::cosf(phi1);
			float cosPhi2 = std::cosf(phi2);
			float sinPhi1 = std::sinf(phi1);
			float sinPhi2 = std::sinf(phi2);

			//phi2   phi1
			// |      |
			// 2------1 -- theta1
			// | \    |
			// |  \   |
			// |   \  |
			// |    \ |
			// 3------4 -- theta2

			// x = r * sin(theta) * cos(phi)
			// y = r * sin(theta) * sin(phi)
			// z = r * cos(phi)

			// phi1 theta1
			float x11 = radius * sinTheta1 * cosPhi1;
			float y11 = radius * sinTheta1 * sinPhi1;
			float z11 = radius * cosTheta1;

			// phi1 theta2
			float x12 = radius * sinTheta2 * cosPhi1;
			float y12 = radius * sinTheta2 * sinPhi1;
			float z12 = radius * cosTheta2;

			// phi2 theta1
			float x21 = radius * sinTheta1 * cosPhi2;
			float y21 = radius * sinTheta1 * sinPhi2;
			float z21 = radius * cosTheta1;

			// phi2 theta2
			float x22 = radius * sinTheta2 * cosPhi2;
			float y22 = radius * sinTheta2 * sinPhi2;
			float z22 = radius * cosTheta2;

			if (i == 0)		// upper cap
			{
				uint32_t v1Index = (uint32_t)vertices.size();
				uint32_t v3Index = (uint32_t)vertices.size() + 1;
				uint32_t v4Index = (uint32_t)vertices.size() + 2;

				vertices.push_back({ { x11, z11, y11, 1.0f }, green });
				vertices.push_back({ { x22, z22, y22, 1.0f }, green });
				vertices.push_back({ { x12, z12, y12, 1.0f }, green });

				indices.push_back(v1Index);
				indices.push_back(v3Index);
				indices.push_back(v4Index);
			}
			else if (i + 1 == latitudeCount)		// lower cap
			{
				uint32_t v1Index = (uint32_t)vertices.size();
				uint32_t v2Index = (uint32_t)vertices.size() + 1;
				uint32_t v4Index = (uint32_t)vertices.size() + 2;

				vertices.push_back({ { x11, z11, y11, 1.0f }, green });
				vertices.push_back({ { x21, z21, y21, 1.0f }, green });
				vertices.push_back({ { x12, z12, y12, 1.0f }, green });

				indices.push_back(v1Index);
				indices.push_back(v2Index);
				indices.push_back(v4Index);
			}
			else
			{
				uint32_t v1Index = (uint32_t)vertices.size();
				uint32_t v4Index = (uint32_t)vertices.size() + 1;
				uint32_t v2Index = (uint32_t)vertices.size() + 2;
				uint32_t v3Index = (uint32_t)vertices.size() + 3;

				vertices.push_back({ { x11, z11, y11, 1.0f }, green });
				vertices.push_back({ { x12, z12, y12, 1.0f }, green });
				vertices.push_back({ { x21, z21, y21, 1.0f }, green });
				vertices.push_back({ { x22, z22, y22, 1.0f }, green });

				indices.push_back(v1Index);
				indices.push_back(v2Index);
				indices.push_back(v4Index);

				indices.push_back(v2Index);
				indices.push_back(v3Index);
				indices.push_back(v4Index);
			}
		}
	}

    Vector4f worldPos(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);

	GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphicComponent->SetIndexBuffer(
		m_Device.Get(),
		indices);

	graphicComponent->SetVertexBuffer(
		m_Device.Get(),
		vertices
	);

    wavefront::AABB boundingBox;
    boundingBox.Set(Vector3<float>(-radius, -radius, -radius), Vector3<float>(radius, radius, radius));
    graphicComponent->SetBoundingBox(boundingBox);
    graphicComponent->SetWorldPosition(worldPos);

	graphicComponent->ChangeWorldViewProjBufferData(
		m_DeviceContext.Get(),
		{ worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });

	m_SphereMesh.SetGraphicsComponent(graphicComponent);
}

void Renderer::SetupOctahedronMesh()
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
        std::move(vertexShaderInputLayout)
    };

    // calculate vertices for the sphere mesh
    float radius = 7.0f;
    Vector4f green = { 0.0f, 1.0f, 0.0f, 1.0f };
    
    int triangulateLevel = 2;
    Octahedron oct(radius);
    oct.triangulate(triangulateLevel);

    std::vector<Vertex> vertices;
    const std::vector<OctahedronVertex>& octVertices = oct.GetVertices();
    for (int i = 0; i < octVertices.size(); ++i)
    {
        vertices.push_back(Vertex(octVertices[i].m_vertex.Mul3(Vector4f(radius, radius, radius, 1.0f)), green));
    }

    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(-30.0f, 0.0f, -30.0f);

    GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphicComponent->SetIndexBuffer(
        m_Device.Get(),
        oct.GetIndices());

    graphicComponent->SetVertexBuffer(
        m_Device.Get(),
        vertices
    );

    wavefront::AABB boundingBox;
    boundingBox.Set(Vector3<float>(-radius, -radius, -radius), Vector3<float>(radius, radius, radius));
    graphicComponent->SetBoundingBox(boundingBox);

    graphicComponent->ChangeWorldViewProjBufferData(
        m_DeviceContext.Get(),
        { worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });

    m_OctahedronMesh.SetGraphicsComponent(graphicComponent);
}

void Renderer::SetupHemioctahedronMesh()
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
        std::move(vertexShaderInputLayout)
    };

    // calculate vertices for the sphere mesh
    float radius = 7.0f;
    Vector4f green = { 0.0f, 1.0f, 0.0f, 1.0f };

    int triangulateLevel = 2;
    static Hemioctahedron oct(radius, triangulateLevel);

    std::vector<Vertex> vertices;
    oct.ProcessVertices([&vertices, green](const OctahedronVertex& octahedronVertex, uint32_t i)
    {
        if(i == 0)
            vertices.push_back(Vertex(octahedronVertex.m_vertex, {1.0f, 0.0f, 0.0f, 1.0f}));
        else
            vertices.push_back(Vertex(octahedronVertex.m_vertex, green));
    });

    Vector4f worldPos(-50.0f, 0.0f, -50.0f, 0.0f);
    DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);

    GraphicsComponent* graphicComponent = new GraphicsComponent(desc);
    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    graphicComponent->SetIndexBuffer(
        m_Device.Get(),
        oct.GetIndices());

    graphicComponent->SetVertexBuffer(
        m_Device.Get(),
        vertices
    );

    wavefront::AABB boundingBox;
    boundingBox.Set(Vector3<float>(-radius, 0.0f, -radius), Vector3<float>(radius, radius, radius));
    graphicComponent->SetBoundingBox(boundingBox);
    graphicComponent->SetWorldPosition(worldPos);

    graphicComponent->ChangeWorldViewProjBufferData(
        m_DeviceContext.Get(),
        { worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });

    m_HemioctahedronMesh.SetGraphicsComponent(graphicComponent);
}

void Renderer::SetupSpaceShip()
{
	wavefront::Obj result = wavefront::ObjLoader::Parse("../../../assets/Models/spaceCraft4.obj");

	GraphicsComponent::GraphicsComponentDesc desc =
	{
		m_Device.Get(),
		L"SpaceshipVertexShader.cso",
		L"SpaceshipPixelShader.cso",
        std::move(result.vertexShaderInputLayout)
	};

	GraphicsComponent* graphicsComponent = new GraphicsComponent(desc);
	graphicsComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    for (auto it = result.perMaterialFaces.begin(); it != result.perMaterialFaces.end(); ++it)
    {
        uint32_t materialID = it->first;
        graphicsComponent->AddVertexBatch(m_Device.Get(), it->second.vertexBuffer, materialID, result.m_topology);
        graphicsComponent->AddMaterial(m_Device.Get(), materialID, result.materials[materialID]);
    }

    graphicsComponent->SetDrawType(result.drawType);
	
    graphicsComponent->LoadTexture(m_Device.Get(), L"../../../assets/uv-checkerboard.png");

	D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    samplerDesc.MipLODBias = 0;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    graphicsComponent->InitSamplerState(m_Device.Get(), samplerDesc);

    Vector4f worldPos(0.0f, 0.0f, 5.0f, 0.0f);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(worldPos.x, worldPos.y, worldPos.z);

    graphicsComponent->SetBoundingBox(result.boundingBox);
    graphicsComponent->SetWorldPosition(worldPos);

	graphicsComponent->ChangeWorldViewProjBufferData(
		m_DeviceContext.Get(),
		{ worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });

	m_SpaceShip.SetGraphicsComponent(graphicsComponent);
}

void Renderer::SetupSpaceShipForRender(bool hasInput)
{
    Vector4f pos(0.0f, 0.0f, -20.0f, 1.0f);
	DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	GraphicsComponent* graphicComponent = m_SpaceShip.GetGraphicsComponent();

	graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	graphicComponent->SetSamplerState(m_DeviceContext.Get());

    graphicComponent->SetWorldPosition(pos);

	if (hasInput)
	{
		graphicComponent->ChangeWorldViewProjBufferData(
			m_DeviceContext.Get(),
			{ worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });
	}
}

void Renderer::SetupPrimitiveForRender(bool hasInput, Primitive prim/*= Triangle*/)
{
	if (prim == Triangle)
	{
        Vector4f pos(1.0f, 0.0f, 2.0f, 1.0f);
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		GraphicsComponent* graphicComponent = m_Cube.GetGraphicsComponent();
		graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        graphicComponent->SetWorldPosition(pos);

		if (hasInput)
		{
			graphicComponent->ChangeWorldViewProjBufferData(
				m_DeviceContext.Get(), 
				{worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix()});
		}
	}
	else if(prim == Line)
	{
        Vector4f pos(0.0f, 0.0f, 0.0f, 1.0f);
		DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		GraphicsComponent* graphicComponent = m_Axis.GetGraphicsComponent();
		graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        graphicComponent->SetWorldPosition(pos);

		if (hasInput)
		{
			graphicComponent->ChangeWorldViewProjBufferData(
				m_DeviceContext.Get(), 
				{worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix()});
		}
	}
	else if(prim == Sphere || prim == Octahedral || prim == Hemioctahedral)
	{
        Vector4f pos;
        if (prim == Sphere)
            pos = Vector4f(20.0f, 0.0, -20.0f, 1.0f);
        else if (prim == Octahedral)
            pos = Vector4f(-20.0f, 0.0f, 0.0f, 1.0f);
        else
            pos = Vector4f(-50.0f, 0.0f, -50.0f, 1.0f);

        DirectX::XMMATRIX worldMatrix = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

        //DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationRollPitchYaw(PI / 20, 0, PI / 20);
        //if (prim == Octahedral) worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, rotation);

		GraphicsComponent* graphicComponent = prim == Sphere ? m_SphereMesh.GetGraphicsComponent() : (prim == Octahedral ? m_OctahedronMesh.GetGraphicsComponent() : m_HemioctahedronMesh.GetGraphicsComponent());
		graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        graphicComponent->SetWorldPosition(pos);

		if (hasInput)
		{
			graphicComponent->ChangeWorldViewProjBufferData(
				m_DeviceContext.Get(),
				{ worldMatrix, m_Camera.GetViewMatrix(), m_Camera.GetProjectionMatrix() });
		}
	}
}

bool Renderer::onInput(InputClass* input, Camera& camera)
{
	if (!input)
	{
		return false;
	}

	float threshHold = 0.05f;
	
	bool hasInput = false;

	if (input->IsKeyDown('W'))
	{
		DirectX::XMVECTOR moveForwardVec = DirectX::XMVectorSet(0.0f, 0.0f, threshHold, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveForwardVec);
				
		//OUTPUT_DEBUG("%f %f %f %f\n", camPos.m128_f32[0], camPos.m128_f32[1], camPos.m128_f32[2], camPos.m128_f32[3]);
		
		hasInput = true;
	}
	if (input->IsKeyDown('S'))
	{
		DirectX::XMVECTOR moveBackwardVec = DirectX::XMVectorSet(0.0f, 0.0f, -threshHold, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveBackwardVec);

		//OUTPUT_DEBUG("%f %f %f %f\n", camPos.m128_f32[0], camPos.m128_f32[1], camPos.m128_f32[2], camPos.m128_f32[3]);

		hasInput = true;
	}
	if (input->IsKeyDown('A'))
	{
		DirectX::XMVECTOR moveLeftVec = DirectX::XMVectorSet(-threshHold, 0.0f, 0.0f, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveLeftVec);

		hasInput = true;
	}
	if (input->IsKeyDown('D'))
	{
		DirectX::XMVECTOR moveRightVec = DirectX::XMVectorSet(threshHold, 0.0f, 0.0f, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveRightVec);

		hasInput = true;
	}
	if (input->IsKeyDown('Q'))
	{
		camera.Rotate(RotationAxis::Yaw, -threshHold);

		hasInput = true;
	}
	if (input->IsKeyDown('E'))
	{
		camera.Rotate(RotationAxis::Yaw, threshHold);

		hasInput = true;
	}
	if (input->IsKeyDown('R'))
	{
		camera.Rotate(RotationAxis::Pitch, -threshHold);
		
		hasInput = true;
	}
	if (input->IsKeyDown('F'))
	{
		camera.Rotate(RotationAxis::Pitch, threshHold);

		hasInput = true;
	}
	if (input->IsKeyDown('T'))
	{
		camera.Rotate(RotationAxis::Roll, threshHold);

		hasInput = true;
	}
	if (input->IsKeyDown('G'))
	{
		camera.Rotate(RotationAxis::Roll, -threshHold);

		hasInput = true;
	}
    if (input->IsKeyDown('B'))
    {
        DebugDisplay::ToggleDebugDisplay();
        hasInput = false;
    }
	else if (input->IsKeyDown(VK_UP))
	{
		DirectX::XMVECTOR moveUpVec = DirectX::XMVectorSet(0.0, threshHold, 0.0f, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveUpVec);

		hasInput = true;
	}
	else if (input->IsKeyDown(VK_DOWN))
	{
		DirectX::XMVECTOR moveDownVec = DirectX::XMVectorSet(0.0f, -threshHold, 0.0f, 0.0f);
		camera.MoveCameraWorldAxisAligned(moveDownVec);

		hasInput = true;
	}
	else if (input->MouseWheelMoved())
	{
		camera.ZoomCamera(input->GetMouseWheelDelta() < 0 ? ZoomType::ZoomOut : ZoomType::ZoomIn);
		hasInput = true;
	}
    else if (input->IsPanning())
    {
        Vector2<float> panDir = input->GetPanningDirection().ToFloatVec() * 0.05f;
        DirectX::XMVECTOR panningVec = DirectX::XMVectorSet((float)panDir[0], (float)panDir[1], 0.0f, 0.0f);
        camera.MoveCameraOrientationAxisAligned(panningVec);
        hasInput = true;
    }
    else if (input->IsRotating())
    {
        Vector2<float> rotDir = input->GetRotatingDirection().ToFloatVec() * 0.10f;
        camera.Rotate(RotationAxis::Yaw, rotDir[0]);
        camera.Rotate(RotationAxis::Pitch, rotDir[1]);
        hasInput = true;
    }

	return hasInput;
}

bool Renderer::TestCubeRotation(InputClass* input, float& xRotation, float& yRotation, float& zRotation)
{
	if (!input)
	{
		return false;
	}

	float threshhold = 1.0f;

	if (input->IsKeyDown('E'))
	{
		xRotation += threshhold;

		OUTPUT_DEBUG("%f\n", xRotation);


		return true;
	}
	else if (input->IsKeyDown('D'))
	{
		xRotation -= threshhold;

		OUTPUT_DEBUG("%f\n", xRotation);
		
		return true;
	}
	else if (input->IsKeyDown('R'))
	{
		yRotation += threshhold;

		OUTPUT_DEBUG("%f\n", yRotation);
		
		return true;
	}
	else if (input->IsKeyDown('F'))
	{
		yRotation -= threshhold;

		OUTPUT_DEBUG("%f\n", yRotation);
		
		return true;
	}
	else if (input->IsKeyDown('T'))
	{
		zRotation += threshhold;

		OUTPUT_DEBUG("%f\n", zRotation);
		
		return true;
	}
	else if (input->IsKeyDown('G'))
	{
		zRotation -= threshhold;

		OUTPUT_DEBUG("%f\n", zRotation);
		
		return true;
	}

	return false;
}
