struct InputPixel
{
	float4 pos : SV_POSITION;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
    float3 worldPos : WORLDPOS;
};

cbuffer MaterialBuffer : register(b0)
{
    float4 ambient;
    float4 diffuseColor;
    float4 specular;
}

cbuffer LightingBuffer : register(b1)
{
    float4 lightSourcePos;
    float3 lightColor;
}

Texture2D<float4> checkerboardTexture : register(t0);
SamplerState wrapSampler : register(s0);

float4 main(InputPixel inputPixel) : SV_TARGET
{
    // calculating ambient lighting
    float ambientStrength = 0.1f;
    float3 ambient = ambientStrength * diffuseColor;
    
    // calculating diffuse lighting
    float3 normal = normalize(inputPixel.normal);
    float3 lightDir = normalize(lightSourcePos.xyz - inputPixel.worldPos);

    float diff = max(dot(normal, lightDir), 0.0f);
    float3 diffuse = diff * lightColor;

    float3 finalColor = (diffuse + ambient) * diffuseColor.rgb;
    

    return float4(finalColor, 1.0f);
}
