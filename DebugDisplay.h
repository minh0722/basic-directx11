#pragma once

#include "Vector3.h"

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
    DebugDisplay(ID3D11Device* device);

    static void SetDebugDisplay(DebugDisplay* debugDisplay) { ms_DebudDisplay = debugDisplay; }
    static DebugDisplay& GetDebugDisplay() { return *ms_DebudDisplay; }

    void Draw3DBox(Vector3<float> pos, Vector3<float> center, Vector3<float> extent);

    void Render(ID3D11DeviceContext* context);

private:
    void Setup3DBoxBuffers(ID3D11Device* device);

private:
    static DebugDisplay* ms_DebudDisplay;

    Debug3DBox m_3DBoxes[MAX_ELEMENT];
    uint32_t m_3DBoxesCount = 0;

    Microsoft::WRL::ComPtr<ID3D11Buffer> m_3DBoxedVertexBuffer;
    Microsoft::WRL::ComPtr<ID3D11Buffer> m_3DBoxedInstanceBuffer;
};