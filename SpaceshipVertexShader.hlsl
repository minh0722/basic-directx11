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
};

struct PixelOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

PixelOutput main(VertexInput inputVertex)
{
	PixelOutput output;

	output.pos = mul(world, float4(inputVertex.pos, 1.0f));
	output.pos = mul(view, output.pos);
	output.pos = mul(proj, output.pos);

	output.color = float4(0.0f, 1.0f, 0.0f, 1.0f);

	return output;
}