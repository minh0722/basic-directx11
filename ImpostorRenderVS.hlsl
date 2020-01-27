
cbuffer WorldViewProj : register(b0)
{
    matrix worldViewProj;
};

cbuffer VertexConstants : register(b1)
{
    matrix worldToObject;
    float4 cameraWorldPos;
};

ByteAddressBuffer VertexDataBuffer : register(t0);

// @param vertexID in range [0; 3]
// returns a unit quad with following vertices <x, y, z>
// 0 - <-0.5f, 0, -0.5f>
// 1 - < 0.5f, 0, -0.5f>
// 2 - < 0.5f, 0,  0.5f>
// 3 - <-0.5f, 0,  0.5f>
float3 VertexIDToQuadVertex(uint vertexId)
{
    return float3(float((vertexId >> 1) ^ (vertexId & 1)) - 0.5f, 0.0f, float((vertexId >> 1) & 1) - 0.5f);
}

float3 ImpostorVertex(float3 vertex, float2 texcoord)
{
    float4 cameraPosObjectSpace = mul(worldToObject, cameraWorldPos);
    

    return float4(0.0f, 0.0f, 0.0f, 0.0f);
}

void main(uint vertexID : SV_VertexID)
{
    float3 vertex = VertexIDToQuadVertex(vertexID);
    float2 uv = VertexDataBuffer.Load2(vertexID * 2 * 4);   // 2 float uv

    float3 cameraFacingQuadVertex = ImpostorVertex(vertex, uv);

}