#include "common.h"

cbuffer ConstBuffer : register(b0)
{
    matrix view;
    matrix proj;
};

struct VertexInput
{
    float4 pos : POSITION;
};

struct PixelOutput
{
    float4 pos : SV_POSITION;
    float4 color : COLOR;
};

PixelOutput main(VertexInput inputVertex, float4 instancePos : INSTANCEPOS0, uint instanceID : SV_InstanceID)
{
    PixelOutput output;
    
    output.pos = instancePos + inputVertex.pos;
    output.pos = mul(view, output.pos);
    output.pos = mul(proj, output.pos);

    output.color = float4(1.0f, 1.0f, 1.0f, 1.0f);

    return output;
}