#include "pch.h"
#include "renderer.h"
#include "GraphicsComponent.h"
#include "Matrix44f.h"
#include "Camera.h"
#include "inputclass.h"
#include <cmath>

float cos45 = std::cos(PI / 4);
float sin45 = std::sin(PI / 4);

Renderer* Renderer::ms_Instance = nullptr;

Renderer::Renderer()
{
}

void Renderer::Initialize(HWND window)
{
	m_Window = window;
    InitDeviceSwapChainAndDeviceContext(window);
	InitRenderTargetView(m_SwapChain.Get());
	InitViewPort();
	
    InitDepthStencilBufferAndView();
    InitDepthStencilState();
	InitRasterizerState();
	
	SetupTriangle();
	SetupCube();
    SetupAxis();
}

void Renderer::Render(InputClass* input)
{
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	m_DeviceContext->ClearRenderTargetView(m_RenderTargetView.Get(), color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);

	//m_Triangle.Render(m_DeviceContext.Get());
    SetupCubeForRender(input);
	m_Cube.Render(m_DeviceContext.Get());
    m_Axis.Render(m_DeviceContext.Get());

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

	const D3D_FEATURE_LEVEL feature[] = { D3D_FEATURE_LEVEL_11_1 };

	THROW_IF_FAILED(
		D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
            D3D11_CREATE_DEVICE_DEBUG,
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
			m_DepthStencilBuffer.GetAddressOf()));

    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc = {};
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    THROW_IF_FAILED(
        m_Device->CreateDepthStencilView(
            m_DepthStencilBuffer.Get(),
            &depthStencilViewDesc,
            m_DepthStencilView.GetAddressOf()));
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
			m_DepthStencilState.GetAddressOf()));

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
        { { 0.0f, 1.0f, 1.0f, 1.0f }, green },

        { { 3.0f, 0.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 3.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 3.0f, 1.0f }, red }
    };

    // do transform
    XMMATRIX translation = XMMatrixTranslation(1.0f, 0.0f, 2.0f);
    
	//Matrix44f viewMatrix = camera.GetViewMatrix();
    static XMVECTOR cameraPos = { 0.0f, 3.0f, 0.0f, 1.0f };
    static XMVECTOR lookAtPos = { 1.0f, 0.0f, 2.0f, 1.0f };
    static float fov = 120.0f;
    
    Matrix44f worldMatrix = Matrix44f(translation /** rotation*/);
    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, { 0.0f, 1.0f, 0.0f, 1.0f });
    XMMATRIX perspectiveProjMatrix = XMMatrixPerspectiveFovLH(fov * RADIAN, (float)screenWidth / (float)screenHeight, 0.0f, 100.0f);
        
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
        vertices
        );
    
    graphicComponent->ChangeWorldViewProjBufferData(
        m_DeviceContext.Get(),
        {worldMatrix.m_matrix, viewMatrix, perspectiveProjMatrix});

	m_Cube.AddComponent(graphicComponent);
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
        vertexShaderInputLayout
    };

    ///////////////////////////////////////////////////////////////////////
    Vector4f red = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vector4f green = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vector4f blue = { 0.0f, 0.0f, 1.0f, 0.0f };

    Matrix44f worldViewProj;

    // left handed coordinate system. Same as directx
    std::vector<Vertex> vertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, red },        // x
        { { 1.0f, 0.0f, 0.0f, 1.0f }, red },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, green },      // y
        { { 0.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, blue },       // z
        { { 1.0f, 0.0f, 1.0f, 1.0f }, blue }
    };

    // do transform
    XMMATRIX translation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);

    Matrix44f worldMatrix = Matrix44f(translation);

    static XMVECTOR cameraPos = { 0.0f, 3.0f, 0.0f, 1.0f };
    static XMVECTOR lookAtPos = { 1.0f, 0.0f, 2.0f, 1.0f };
    static float fov = 120.0f;

    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, { 0.0f, 1.0f, 0.0f, 1.0f });

    worldViewProj = worldMatrix * viewMatrix;

    XMMATRIX perspectiveProjMatrix = XMMatrixPerspectiveFovLH(fov * RADIAN, (float)screenWidth / (float)screenHeight, 0.0f, 100.0f);

    worldViewProj = worldViewProj * perspectiveProjMatrix;

    for (size_t i = 0; i < vertices.size(); ++i)
    {
        // multiply by world view proj matrix and divide by w
        //XMVECTOR pos = XMVector3TransformCoord(vertices[i].pos.m_v, worldViewProj.m_matrix);

        Vector4f pos = (vertices[i].pos * worldViewProj);
        pos = pos / pos.w;

        vertices[i].pos = pos;
    }

    ///////////////////////////////////////////////////////////////////////

    BaseComponent* graphicComponent = new GraphicsComponent(desc);
    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
    graphicComponent->SetIndexBuffer(
        m_Device.Get(),
        { 0, 1, 2, 3, 4, 5 });

    graphicComponent->SetVertexBuffer(
        m_Device.Get(),
        vertices
    );

    m_Axis.AddComponent(graphicComponent);
}

void Renderer::SetupCubeForRender(InputClass* input)
{
    ///////////////////////////////////////////////////////////////////////
    Vector4f red = { 1.0f, 0.0f, 0.0f, 0.0f };
    Vector4f green = { 0.0f, 1.0f, 0.0f, 0.0f };
    Vector4f blue = { 0.0f, 0.0f, 1.0f, 0.0f };

    Matrix44f worldViewProj;

/*
    // color shifting animation
    {
        static float count = 0.1f;
        float transitionColor = 0.5f + 0.5f * sin(count);
        count += 0.001;

        //red = { 0.5f, 0.3f, transitionColor, 0.0f};
        //green = { 0.2f, transitionColor, 0.6f, 0.0f};
        //blue = { transitionColor, 0.3f, 0.5f, 0.0f};

        red = { transitionColor, 0.3f, 0.4f, 0.0f };
        green = { 0.2f, transitionColor, 0.6f, 0.0f };
        blue = { 0.5f, 0.3f, transitionColor, 0.0f };

    }
*/

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
        { { 0.0f, 1.0f, 1.0f, 1.0f }, green },

        { { 3.0f, 0.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 3.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 3.0f, 1.0f }, red }
    };

    // do transform
    XMMATRIX translation = XMMatrixTranslation(1.0f, 0.0f, 2.0f);

    // rotation 45 degrees around y axis
    //XMMATRIX rotation = XMMatrixRotationY(45);
        
    //Matrix44f viewMatrix = camera.GetViewMatrix();
    static XMVECTOR cameraPos = { 0.0f, 3.0f, 0.0f, 1.0f };
    static XMVECTOR lookAtPos = { 1.0f, 0.0f, 2.0f, 1.0f };
    static float fov = 120.0f;
    
    bool hasInput = onInput(input, cameraPos, lookAtPos, fov);

    Matrix44f worldMatrix = Matrix44f(translation /** rotation*/);
    XMMATRIX viewMatrix = XMMatrixLookAtLH(cameraPos, lookAtPos, { 0.0f, 1.0f, 0.0f, 1.0f });
    XMMATRIX perspectiveProjMatrix = XMMatrixPerspectiveFovLH(fov * RADIAN, (float)screenWidth / (float)screenHeight, 0.0f, 100.0f);
        
    GraphicsComponent* graphicComponent = m_Cube.GetGraphicsComponent();

    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    
    if (hasInput)
    {
        graphicComponent->ChangeIndexBufferData(
            m_DeviceContext.Get(),
            { 0, 1, 4, 1, 5, 4,
              1, 2, 5, 5, 2, 6,
              7, 4, 5, 7, 5, 6,
              3, 1, 0, 3, 2, 1,
              7, 6, 2, 7, 2, 3,
              7, 3, 4, 4, 3, 0 });
        
        graphicComponent->ChangeVertexBufferData(
            m_DeviceContext.Get(),
            vertices
            );

        graphicComponent->ChangeWorldViewProjBufferData(
            m_DeviceContext.Get(),
            { worldMatrix.m_matrix, viewMatrix, perspectiveProjMatrix });
    }

    ///////////////////////////////////////////////////////////////////////
    return;
    /* NOW UPDATING THE AXIS. IT'S UGLY TO UPDATE HERE, WILL REIMPLEMENT EVERYTHING ONCE THIS WORKS */

    std::vector<Vertex> axisVertices =
    {
        { { 0.0f, 0.0f, 0.0f, 1.0f }, red },        // x
        { { 1.0f, 0.0f, 0.0f, 1.0f }, red },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, green },      // y
        { { 0.0f, 1.0f, 0.0f, 1.0f }, green },
        { { 0.0f, 0.0f, 0.0f, 1.0f }, blue },       // z
        { { 0.0f, 0.0f, 1.0f, 1.0f }, blue }
    };
    
    XMMATRIX axisTranslation = XMMatrixTranslation(0.0f, 0.0f, 0.0f);
    Matrix44f axisWorldMatrix = Matrix44f(translation);
    worldViewProj = axisWorldMatrix * viewMatrix;
    worldViewProj = worldViewProj * perspectiveProjMatrix;

    for (size_t i = 0; i < axisVertices.size(); ++i)
    {
        // multiply by world view proj matrix and divide by w
        //XMVECTOR pos = XMVector3TransformCoord(vertices[i].pos.m_v, worldViewProj.m_matrix);

        Vector4f pos = (axisVertices[i].pos * worldViewProj);
        pos = pos / pos.w;

        axisVertices[i].pos = pos;
    }

    graphicComponent = m_Axis.GetGraphicsComponent();
    graphicComponent->SetPrimitiveTopology(m_DeviceContext.Get(), D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

    if (hasInput)
    {
        graphicComponent->ChangeIndexBufferData(
            m_DeviceContext.Get(),
            { 0, 1, 2, 3, 4, 5 });

        graphicComponent->ChangeVertexBufferData(
            m_DeviceContext.Get(),
            vertices
        );
    }
}

bool Renderer::onInput(InputClass* input, XMVECTOR& cameraPos, XMVECTOR& lookAtPos, float& fov)
{
    if (!input)
    {
        return false;
    }

    bool hasInput = false;
    float threshHold = 0.001f;

    //// increase far plane dist by 0.1f
    //if (input->IsKeyDown('Q'))
    //{
    //    m_FarPlaneDist == 0.1f;
    //}
    //// decrease far plane dist
    //else if (input->IsKeyDown('A'))
    //{
    //    m_FarPlaneDist -= 0.1f;
    //}

    float* camPos = reinterpret_cast<float*>(&cameraPos);

    
    {
        XMVECTOR lookatNormalized = XMVector3Normalize(lookAtPos);
        
        if (input->IsKeyDown('W'))
        {
            XMVECTOR moveForwardVec = XMVectorScale(lookAtPos, threshHold);
            cameraPos = XMVectorAdd(cameraPos, moveForwardVec);
            cameraPos.m128_f32[3] = 1.0f;
            
            char buf[256];
            snprintf(buf, 256, "%f %f %f %f\n", cameraPos.m128_f32[0], cameraPos.m128_f32[1], cameraPos.m128_f32[2], cameraPos.m128_f32[3]);
            OutputDebugStringA(buf);

            return true;
        }
        else if (input->IsKeyDown('S'))
        {
            XMVECTOR moveBackwardVec = XMVectorScale(lookAtPos, -threshHold);
            cameraPos = XMVectorAdd(cameraPos, moveBackwardVec);
            cameraPos.m128_f32[3] = 1.0f;
            
            char buf[256];
            snprintf(buf, 256, "%f %f %f %f\n", cameraPos.m128_f32[0], cameraPos.m128_f32[1], cameraPos.m128_f32[2], cameraPos.m128_f32[3]);
            OutputDebugStringA(buf);

            return true;
        }
        //else if (input->IsKeyDown('D'))
        //{
        //    
        //}
        //else if (input->IsKeyDown('A'))
        //{
        //    
        //}
        return false;
    }
    

    //// increase fov
    //if (input->IsKeyDown('W'))
    //{
    //    fov += threshHold;
    //}
    //// decrease fov
    //else if (input->IsKeyDown('S'))
    //{
    //    fov -= threshHold;
    //    if (fov < 0.00001f)
    //    {
    //        fov = 0.1f;
    //    }
    //}        
    // increase camera x position
    if (input->IsKeyDown(unsigned int('E')))
    {
        camPos[0] += threshHold;
        hasInput = true;
    }
    // derease camera x position
    else if (input->IsKeyDown('D'))
    {
        camPos[0] -= threshHold;
        hasInput = true;
    }
    // increase camera y position
    else if (input->IsKeyDown('R'))
    {
        camPos[1] += threshHold;
        hasInput = true;
    }
    // decrease camera y position
    else if (input->IsKeyDown('F'))
    {
        camPos[1] -= threshHold;
        hasInput = true;
    }
    // increase camera z position
    else if (input->IsKeyDown('T'))
    {
        camPos[2] += threshHold;
        hasInput = true;
    }
    // decrease camera z position
    else if (input->IsKeyDown('G'))
    {
        camPos[2] -= threshHold;
        hasInput = true;
    }

    return hasInput;
}