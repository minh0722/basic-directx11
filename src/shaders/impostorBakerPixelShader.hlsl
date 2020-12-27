struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
};

struct InputPixel
{
    float4 pos : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
};

cbuffer MaterialBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
}

SamplerState wrapSampler : register(s0);

PS_OUTPUT main(InputPixel inputPixel)
{
    PS_OUTPUT output;
    output.color = float4(diffuse.rgb, 1.0f);
    output.normal = float4(inputPixel.normal * 0.5f + 0.5f, 1.0f - inputPixel.pos.z);

    return output;
}
