#include "common.h"

cbuffer ConstBuffer : register(b0)
{
    matrix view;
    matrix proj;
};

struct VertexInput
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VertexOutput
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VertexOutput main(VertexInput inputVertex, uint vertexId : SV_VertexID)
{
    VertexOutput output;

    output.pos = mul(view, inputVertex.pos);
    output.pos = mul(proj, output.pos);

    output.uv = inputVertex.uv;

    return output;
}