#include "computeShadersCommon.h"

cbuffer Constants : register(b0)
{
    uint frameCount;
    uint frameX;
    uint frameY;
};

Texture2D<float4> source : register(t0);
RWStructuredBuffer<float> MinDistances : register(u0);
RWStructuredBuffer<float> MaxDistances : register(u1);
RWTexture2D<float4> Result : register(u2);

[numthreads(GROUP_SIZE, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint width;
    uint height;
    source.GetDimensions(width, height);

    uint frameSize = width / frameCount;

    uint x = idx % frameSize;
    uint y = idx / frameSize;

    x += frameX * frameSize;
    y += frameY * frameSize;

    x %= (x + frameSize);
    y %= (y + frameSize);

    float4 color = source.Load(uint3(x, y, 0));
    color.a = saturate(MinDistances[y * width + x] / (MaxDistances[0] * 0.5f));
    MinDistances[0] = 0.0f;

    if (x == 0 || x == frameX * frameSize - 1 || y == 0 || y == frameY * frameSize - 1)
        color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    Result[uint2(x, y)] = color;
}