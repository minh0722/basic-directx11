#include "computeShadersCommon.h"

Texture2D<float4> normal : register(t0);
Texture2D<float> depth : register(t1);
RWTexture2D<float4> normalDepth : register(u0);

[numthreads(GROUP_SIZE, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint atlasWidth;
    uint atlasHeight;
    normal.GetDimensions(atlasWidth, atlasHeight);

    uint x = idx % atlasHeight;
    uint y = (idx - x) / atlasWidth;

    float4 n = normal.Load(uint3(x, y, 0));
    float d = depth.Load(uint3(x, y, 0));
    
    normalDepth[uint2(x, y)] = float4(n.xyz, 1.0f - d);
}