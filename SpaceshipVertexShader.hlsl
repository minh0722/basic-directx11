#include "common.h"

cbuffer ConstBuffer : register(b0)
{
	matrix world;
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
	float4 color : COLOR;
};

VertexOutput main(VertexInput inputVertex, uint vertexId : SV_VertexID)
{
	VertexOutput output;

	output.pos = mul(world, float4(inputVertex.pos, 1.0f));
	output.pos = mul(view, output.pos);
	output.pos = mul(proj, output.pos);
    
    // 0, 3 -> (1, 0, 0)
    // 1, 4 -> (0, 1, 0)
    // 2, 5 -> (0, 0, 1)
    uint remainder = vertexId % 3;
    if (remainder == 0)
        output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (remainder == 1)
        output.color = float4(0.0f, 1.0f, 0.0f, 1.0f);
    else
        output.color = float4(0.0f, 0.0f, 1.0f, 1.0f);

    output.uv = inputVertex.uv;

    return output;
}