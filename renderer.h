#pragma once
#include "pch.h"
#include "Shape.h"
#include "Camera.h"
#include "Vector4f.h"

class InputClass;

struct Vertex
{
	Vector4f pos;
	Vector4f color;

	Vertex(const Vector4f& pos, const Vector4f& color)
		: pos(pos), color(color) {}

	std::vector<D3D11_INPUT_ELEMENT_DESC> GetLayout()
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> layouts(2);
		layouts[0].SemanticName = "POSITION";
		layouts[0].SemanticIndex = 0;								// will use POSITION0 semantic
		layouts[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// format of the input vertex
		layouts[0].InputSlot = 0;									// 0 ~ 15
		layouts[0].AlignedByteOffset = 0;
		layouts[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;	// per vertex (per instance if for each triangle)
		layouts[0].InstanceDataStepRate = 0;						// number of instances to draw using the same per-instance data before advancing in the buffer by one element

		layouts[1].SemanticName = "COLOR";
		layouts[1].SemanticIndex = 0;
		layouts[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		layouts[1].InputSlot = 0;
		layouts[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		layouts[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		layouts[1].InstanceDataStepRate = 0;

		return layouts;
	}
};

class Renderer
{
public:
	Renderer();
	~Renderer();

    static Renderer& GetInstance()
    {
        // not thread safe
        if (!ms_Instance)
        {
            ms_Instance = new Renderer;
        }

        return *ms_Instance;
    }

	void Initialize(HWND window);
	
	void Render(InputClass* input);

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetContext();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

private:
	void InitDeviceSwapChainAndDeviceContext(HWND window);
	void InitRenderTargetView(IDXGISwapChain* swapChain);
	void InitDepthStencilBufferAndView();
	void InitDepthStencilState();
	void InitRasterizerState(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);
	void InitViewPort();

	void SetupTriangle();
	void SetupCube();
    void SetupAxis();
	void SetupSphereMesh();
	void SetupSpaceShip();
    void SetupOctahedronMesh();
    void SetupHemioctahedronMesh();

	enum Primitive
	{
		Triangle,
		Line,
		Sphere,
        Octahedral,
        Hemioctahedral
	};

    void SetupPrimitiveForRender(InputClass* input, Primitive prim = Triangle);
	void SetupSpaceShipForRender(InputClass* input);

	bool onInput(InputClass* input, Camera& camera);

	bool TestCubeRotation(InputClass* input, float& xRotation, float& yRotation, float& zRotation);

private:
    static Renderer* ms_Instance;

	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_RenderTargetView;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_DepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_DepthStencilView;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_RasterizerState;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_DepthStencilBuffer;
	HWND m_Window;

	Shape m_Triangle;
	Shape m_Cube;
    Shape m_Axis;
	Shape m_SphereMesh;
	Shape m_SpaceShip;
    Shape m_OctahedronMesh;
    Shape m_HemioctahedronMesh;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_uvCheckerboardTexture;

    Camera m_Camera;
};

#define g_Renderer Renderer::GetInstance()