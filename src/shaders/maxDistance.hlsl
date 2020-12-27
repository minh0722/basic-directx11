#include "computeShadersCommon.h"

cbuffer Constants : register(b0)
{
    uint atlasWidth;
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
    // size of one row of views. For 4096x4096 atlas with 10 views -> each view is 409x409
    // so each view row has 4096 atlas width * 409 height per view
    uint rowSize = frameDimension * atlasWidth;
    float maxDist = 0.0f;

    // beginning index of each frame in the buffer
    uint beginIdx = frameY * rowSize + frameX * frameDimension;
    for(uint r = 0; r < frameDimension; ++r)
    for(uint c = 0; c < frameDimension; ++c)
    {
        uint index = frameY * rowSize + r * atlasWidth + frameX * frameDimension + c;
        maxDist = max(maxDist, MinDistances[index]);
    }

    MaxDistances[0] = maxDist;
}