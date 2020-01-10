#include "computeShadersCommon.h"

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint width;
    uint height;
    source.GetDimensions(width, height);

    uint x = idx % height;
    uint y = (idx - x) / width;


}