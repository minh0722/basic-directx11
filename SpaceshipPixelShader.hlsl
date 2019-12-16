struct InputPixel
{
	float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
	float4 color : COLOR;
};

Texture2D<float4> checkerboardTexture : register(t0);
SamplerState wrapSampler : register(s0);

float4 main(InputPixel inputPixel) : SV_TARGET
{
    return checkerboardTexture.Sample(wrapSampler, inputPixel.uv);
}
