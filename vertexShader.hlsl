#include "common.h"

cbuffer ConstBuffer : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

struct VertexInput
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct PixelOutput
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

#if INSTANCING_RENDER
PixelOutput main(VertexInput inputVertex, uint instanceId : SV_INSTANCEID)
#else
PixelOutput main(VertexInput inputVertex)
#endif
{
	PixelOutput output;

#if INSTANCING_RENDER
    float xAxis = instanceId / 100;
    float yAxis = instanceId % 100;

    output.pos = mul(world, float4(inputVertex.pos.x + 10.0f * xAxis, inputVertex.pos.y, inputVertex.pos.z + 10.0f * yAxis, inputVertex.pos.w));
#else
    output.pos = mul(world, inputVertex.pos);
#endif

    output.pos = mul(view, output.pos);
    output.pos = mul(proj, output.pos);

	output.color = inputVertex.color;

	return output;
}