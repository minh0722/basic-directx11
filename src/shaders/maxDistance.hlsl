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

//// do a reduction max operation on the buffer
//// based on https://www.nvidia.com/content/GTC-2010/pdfs/2260_GTC2010.pdf

//groupshared float sharedDistanceData[GROUP_SIZE];

//[numthreads(GROUP_SIZE, 1, 1)]
//void CS_Main(uint3 threadIdx : SV_GroupThreadID, // id of thread in group
//             uint3 groupIdx : SV_GroupID)          // id of group
//{
//    uint dispatchDim = (CB0.workSize + GROUP_SIZE - 1) / GROUP_SIZE;

//    uint groupThreadId = threadIdx.x;
//    uint globalThreadId = groupIdx.x * (GROUP_SIZE * 2) + threadIdx.x;
//    uint dispatchSize = GROUP_SIZE * 2 * dispatchDim;
//    sharedDistanceData[groupThreadId] = 0;
//    GroupMemoryBarrierWithGroupSync();

//    // do reduction in shared mem
//    while (globalThreadId < CB0.workSize)
//    {
//        sharedDistanceData[groupThreadId] = max(MinDistances[globalThreadId], MinDistances[globalThreadId + GROUP_SIZE]);
//        globalThreadId += dispatchSize;
//    }
//    GroupMemoryBarrierWithGroupSync();

//    if (GROUP_SIZE >= 512)
//    {
//        if (groupThreadId < 256)
//        {
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 256]);
//        }
//        GroupMemoryBarrierWithGroupSync();
//    }
//    if (GROUP_SIZE >= 256)
//    {
//        if (groupThreadId < 128)
//        {
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 128]);
//        }
//        GroupMemoryBarrierWithGroupSync();
//    }
//    if (GROUP_SIZE >= 128)
//    {
//        if (groupThreadId < 64)
//        {
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 64]);
//        }
//        GroupMemoryBarrierWithGroupSync();
//    }

//    if (groupThreadId < 32)
//    {
//        if (GROUP_SIZE >= 64)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 32]);
//        if (GROUP_SIZE >= 32)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 16]);
//        if (GROUP_SIZE >= 16)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 8]);
//        if (GROUP_SIZE >= 8)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 4]);
//        if (GROUP_SIZE >= 4)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 2]);
//        if (GROUP_SIZE >= 2)
//            sharedDistanceData[groupThreadId] = max(sharedDistanceData[groupThreadId], sharedDistanceData[groupThreadId + 1]);
//    }

//    // write the result for this block (using group idx)
//    if (groupThreadId == 0)
//        MaxDistances[groupIdx.x] = sharedDistanceData[0];
//}
