#include "computeShadersCommon.h"

cbuffer Constants : register(b0)
{
    uint frameCount;
    uint frameX;
    uint frameY;
};

Texture2D<float4> sourceMask : register(t0);
RWStructuredBuffer<float> MinDistances : register(u0); //size of atlas dimension square

[numthreads(GROUP_SIZE, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint width;
    uint height;
    sourceMask.GetDimensions(width, height);

    uint frameSize = width / frameCount;

    uint x = idx % frameSize;
    uint y = idx / frameSize;

    x += frameX * frameSize;
    y += frameY * frameSize;

    x %= (x + frameSize);
    y %= (y + frameSize);

    float2 origin = float2(x, y);
    uint regionSamples = frameSize * frameSize;

    float centerMask = sourceMask.Load(uint3(x, y, 0)).r;
    float minDistance = frameSize * frameSize;

    if (centerMask > 0.0f)
    {
        uint xStart = frameX * frameSize;
        uint yStart = frameY * frameSize;

        // check all pixels within range
        for (uint r = 0; r < frameSize; ++r)
        for (uint c = 0; c < frameSize; ++c)
        {
            float2 coord = float2(xStart + r, yStart + c);
            float len = length(coord - origin);

            float mask = sourceMask.Load(uint3(coord, 0)).r;
            if (mask < 1)
                minDistance = min(minDistance, len);
        }

        MinDistances[y * width + x] = minDistance;
    }
}