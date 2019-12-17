struct InputPixel
{
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
	float4 color : COLOR;
};

cbuffer MaterialBuffer : register(b0)
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
}

Texture2D<float4> checkerboardTexture : register(t0);
SamplerState wrapSampler : register(s0);

float4 main(InputPixel inputPixel) : SV_TARGET
{
    return float4(diffuse.rgb, 1.0f);
}
