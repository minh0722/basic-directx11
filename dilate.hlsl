#include "computeShadersCommon.h"

#define NEIGHBORS 8

Texture2D<float4> source;
Texture2D<float4> sourceMask;
RWTexture2D<float4> result;
bool AllChannels = false;
bool NormalsDepth = false; // if true, alpha border uses 0.5 instead of 0.0

[numthreads(GROUP_SIZE, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint width;
    uint height;
    source.GetDimensions(width, height);

    uint x = idx % height;
    uint y = (idx - x) / width;

    uint2 offsets[NEIGHBORS] = {
        uint2(-1,  0),
        uint2( 1,  0),
        uint2( 0,  1),
        uint2( 0, -1),
        uint2(-1,  1),
        uint2( 1,  1),
        uint2( 1, -1),
        uint2(-1, -1)
    };

    float4 color = source.Load(uint3(x, y, 0));
    float mask = sourceMask.Load(uint3(x, y, 0)).r;

    if (mask < 1.0f)
    {
        bool exit = false;

        for (int s = 1; s < 32; ++s)
        {
            for (int n = 0; n < NEIGHBORS; ++n)
            {
                // check neighbor
                uint2 of = offsets[n];

                uint3 xyz = uint3(x + (of.x * s), y + (of.y * s), 0);
                float neighborMask = sourceMask.Load(xyz).r;

                // if neighbor filled use neighbor
                if (neighborMask > 0.0f)
                {
                    float4 neighborColor = source.Load(xyz);
                    color.rgb = neighborColor.rgb;
                    if (AllChannels)
                        color.a = neighborColor.a;

                    exit = true;
                    break;
                }
            }
            if (exit)
                break;
        }
    }

    // clear out 1 pixel border
    if (x == 0 || x == width - 1 || y == 0 || y == width - 1)
    {
        if (NormalsDepth)
            color = float4(0.0f, 0.0f, 0.0f, 0.5f);
        else
            color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    result[uint2(x, y)] = color;
}