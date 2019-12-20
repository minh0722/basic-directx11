#include "common.h"

//cbuffer VertexBuffer : register(b0)
//{
//    float4 vertexPos[12 * 2 * 1024];
//}
//
//cbuffer InstanceBuffer : register(b1)
//{
//    matrix worldMatrix[1024];
//}

ByteAddressBuffer VertexBuffer : register(t0);
ByteAddressBuffer InstanceBuffer : register(t1);

cbuffer ConstBuffer : register(b0)
{
    matrix view;
    matrix proj;
};

//struct VertexInput
//{
//    float4 pos : POSITION;
//    float4 instancePos : INSTANCEPOS0;
//};

struct PixelOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PixelOutput main(/*VertexInput inputVertex,*/uint vertexID : SV_VertexID, uint instanceID : SV_InstanceID)
{
    uint vertexAddress = 12 * 2 * instanceID + vertexID * 4;
    float4 vertexPos = asfloat(VertexBuffer.Load4(vertexAddress));

    uint instanceWorldMatrixAddress = instanceID * 16 * 4;  // 16 floats per matrix * 4 bytes per float
    matrix worldMatrix = {
        asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress)),
        asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 16)),
        asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 32)),
        asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 48))
    };

    PixelOutput output;
    
    output.pos = mul(worldMatrix, vertexPos);
    output.pos = mul(view, output.pos);
    output.pos = mul(proj, output.pos);

    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return output;
}