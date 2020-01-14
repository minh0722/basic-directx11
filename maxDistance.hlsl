cbuffer Constants : register(b0)
{
    uint minDistLength;
};

RWStructuredBuffer<float> MinDistances : register(u0);
RWStructuredBuffer<float> MaxDistances : register(u1);  // length of 1

[numthreads(1, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    float maxDist = 0.0f;

    for (uint i = 0; i < minDistLength; ++i)
        maxDist = max(maxDist, MinDistances[i]);

    MaxDistances[0] = maxDist;
}