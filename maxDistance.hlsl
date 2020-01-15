#include "computeShadersCommon.h"

cbuffer Constants : register(b0)
{
    uint minDistLength;
    uint frameDimension;
    uint frameCount;
    uint frameX;
    uint frameY;
};

RWStructuredBuffer<float> MinDistances : register(u0);
RWStructuredBuffer<float> MaxDistances : register(u1);  // length of 1

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint count = frameDimension * frameDimension;
    uint idx = frameX * count + frameY * count;
    float maxDist = 0.0f;

    for (uint i = idx; i < idx + minDistLength; ++i)
        maxDist = max(maxDist, MinDistances[i]);

    MaxDistances[0] = maxDist;
}