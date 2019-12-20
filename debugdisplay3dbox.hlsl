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
    uint vertexAddress = instanceID * 12 * 2 * 16 + vertexID * 16;
    float4 vertexPos = asfloat(VertexBuffer.Load4(vertexAddress));

    uint instanceWorldMatrixAddress = instanceID * 16 * 4;  // 16 floats per matrix * 4 bytes per float

    float4 c0 = asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress));
    float4 c1 = asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 16));
    float4 c2 = asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 32));
    float4 c3 = asfloat(InstanceBuffer.Load4(instanceWorldMatrixAddress + 48));

    matrix worldMatrix = {
        float4(c0.x, c1.x, c2.x, c3.x),
        float4(c0.y, c1.y, c2.y, c3.y),
        float4(c0.z, c1.z, c2.z, c3.z),
        float4(c0.w, c1.w, c2.w, c3.w)
    };

    PixelOutput output;
    
    output.pos = mul(worldMatrix, vertexPos);
    output.pos = mul(view, output.pos);
    output.pos = mul(proj, output.pos);

    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return output;
}