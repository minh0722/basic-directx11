Texture2D<float4> atlas : register(t0);
RWTexture2D<float4> result : register(u0);

[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{

}