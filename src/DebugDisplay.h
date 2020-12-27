#pragma once

#include "Vector3.h"

class Renderer;

struct Debug3DBox
{
    Vector3<float> m_pos;
    Vector3<float> m_center;
    Vector3<float> m_extent;
};

const uint32_t MAX_ELEMENT = 1024;

class DebugDisplay
{
public:
    DebugDisplay(ID3D11Device* device, ID3D11DeviceContext* context);

    static void SetDebugDisplay(DebugDisplay* debugDisplay) { ms_DebudDisplay = debugDisplay; }
    static DebugDisplay& GetDebugDisplay() { return *ms_DebudDisplay; }

    static void ToggleDebugDisplay(bool isEnabled);
    static bool IsEnabled();

    void Draw3DBox(Vector3<float> pos, Vector3<float> center, Vector3<float> extent);

    void Render(Renderer* renderer);

    void OnNewFrame();

private:
    void Setup3DBoxesRenderState(ID3D11Device* device);
    void Setup3DBoxBuffers(ID3D11Device* device);
	void Update3DBoxBuffers(ID3D11DeviceContext* context);

    void SetupViewProjectionBuffer(ID3D11Device* device);
    void UpdateViewProjectionBuffer(Renderer* renderer);

private:
    static DebugDisplay* ms_DebudDisplay;
    static bool ms_EnableDebugDisplay;

    Debug3DBox m_3DBoxes[MAX_ELEMENT];
    uint32_t m_3DBoxesCount = 0;

    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_3DBoxVS;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_3DBoxPS;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_3DBoxesVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_3DBoxesInstanceBuffer;

    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_3DBoxesVertexSRV;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_3DBoxesInstanceSRV;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_CameraViewProjectionBuffer;
};