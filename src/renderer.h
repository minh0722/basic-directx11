#pragma once
#include "pch.h"
#include "Shape.h"
#include "Camera.h"
#include "Vector4f.h"
#include "Vector3.h"
#include "ImguiRenderer.h"
#include "ShaderCompiler.h"

class InputClass;
class DebugDisplay;

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

struct LightSourceSettings
{
    Vector4f m_lightPos;
    Vector4f m_cameraPos;
    Vector3<float> m_lightColor;
    float m_ambientStrength;
    float m_shininess;
};

class Renderer
{
public:
	Renderer();
	~Renderer();

    static Renderer& GetInstance();
    ShaderCompiler& GetShaderCompiler();

	void Initialize(HWND window);
	
	void Render(InputClass* input);

    ID3D11Device* GetDevice();
    ID3D11DeviceContext* GetContext();
    const Camera& GetCamera();

	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

    void SetRasterizerState(D3D11_FILL_MODE fillMode = D3D11_FILL_SOLID, D3D11_CULL_MODE cullMode = D3D11_CULL_BACK);
    void AddShapeToBakeImpostor();
    void SetGlobalLightPosition(float x, float y, float z);
    void SetGlobalLightColor(float r, float g, float b);
    void SetGlobalLightAmbient(float ambientStrength);
    void SetShininess(float shininess);
    const LightSourceSettings& GetGlobalLightSettings() const;

private:
	void InitDeviceSwapChainAndDeviceContext(HWND window);
	void InitRenderTargetView(IDXGISwapChain* swapChain);
	void InitDepthStencilBufferAndView();
	void InitDepthStencilState();
	void SetViewPort();

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

    void SetupPrimitiveForRender(bool hasInput, Primitive prim = Triangle);
	void SetupSpaceShipForRender(bool hasInput);

    void SetupLightingBuffer();
    void SetLightingBuffer();

	bool onInput(InputClass* input, Camera& camera);

private:
    static Renderer* ms_Instance;
    DebugDisplay* m_DebugDisplay;

    ShaderCompiler m_ShaderCompiler;

	Microsoft::WRL::ComPtr<ID3D11Device> m_Device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_DeviceContext;

	Microsoft::WRL::ComPtr<IDXGISwapChain> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_BackBufferRTV;
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

    std::vector<Shape*> m_impostorBakeQueue;

    Camera m_Camera;

    ImguiRenderer m_imguiRenderer;

    // global lighting source
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_GlobalLightingBuffer;
    LightSourceSettings m_GlobalLightSetting;
};

#define g_Renderer Renderer::GetInstance()