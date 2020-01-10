#define GROUP_SIZE 256
#define MAX_DIM_GROUPS 1024
#define MAX_DIM_THREADS (GROUP_SIZE * MAX_DIM_GROUPS)

Texture2D<float4> atlas : register(t0);
RWTexture2D<float4> result : register(u0);

[numthreads(GROUP_SIZE, 1, 1)]
void main( uint3 id : SV_DispatchThreadID )
{
    uint idx = id.x + id.y * MAX_DIM_THREADS;

    uint atlasWidth;
    uint atlasHeight;
    atlas.GetDimensions(atlasWidth, atlasHeight);

    uint x = idx % atlasHeight;
    uint y = (idx - x) / atlasWidth;

    float4 color = atlas.Load(uint3(x, y, 0));
    float hasColor = float(any(color));

    result[uint2(x, y)] = float4(hasColor, 0.0f, 0.0f, 0.0f);
}