#include "ImpostorShaderCommon.cginc"

cbuffer PixelConstants : register(b0)
{
    matrix WorldMatrix;
    float FramesCount;
    float AtlasDimension;
    float Cutoff;
};

Texture2D<float4> impostorNormalAtlas : register(t0);
Texture2D<float4> impostorBaseAtlas : register(t1);
SamplerState impostorSampler : register(s0);

float4 ImpostorBlendWeights(Texture2D<float4> atlas, SamplerState atlasSampler, float2 uv, float2 frame0, float2 frame1, float2 frame2, float4 weights, float2 ddxy)
{
    float4 samp0 = atlas.SampleGrad(atlasSampler, frame0, ddxy.xx, ddxy.yy);
    float4 samp1 = atlas.SampleGrad(atlasSampler, frame1, ddxy.xx, ddxy.yy);
    float4 samp2 = atlas.SampleGrad(atlasSampler, frame2, ddxy.xx, ddxy.yy);

    float4 result = samp0 * weights.x + samp1 * weights.y + samp2 * weights.z;

    return result;
}

void ImpostorSample(in ImpostorData imp, out float4 baseTex, out float4 worldNormal)
{
    float2 fracGrid = frac(imp.grid);
    float4 weights = TriangleInterpolate(fracGrid);

    float2 gridSnap = floor(imp.grid) / FramesCount;

    float2 frame0 = gridSnap;
    float2 frame1 = gridSnap + (lerp(float2(0.0f, 1.0f), float2(1.0f, 0.0f), weights.w) / FramesCount);
    float2 frame2 = gridSnap + (float2(1.0f, 1.0f) / FramesCount);

    float2 vp0uv = frame0 + imp.frame0.xy;
    float2 vp1uv = frame1 + imp.frame1.xy;
    float2 vp2uv = frame2 + imp.frame2.xy;

    // resolution of atlas
    float textureDims = AtlasDimension;
    // fractional frame size, ex 2048 / 12 = 170.6
    float frameSize = textureDims / FramesCount;
    // actual atlas resolution used, ex 170 * 12 = 2040
    float actualDims = floor(frameSize) * FramesCount;
    // the scale factor to apply to UV coord, ex 2048 / 2040 = 0.99609375
    float scaleFactor = actualDims / textureDims;

    vp0uv *= scaleFactor;
    vp1uv *= scaleFactor;
    vp2uv *= scaleFactor;

    float texelSize = 1.0f / AtlasDimension;
    float borderClamp = 2.0f;

    // clamp out neighboring frames
    float2 gridSize = 1.0f / FramesCount;
    gridSize *= AtlasDimension;
    gridSize *= texelSize;
    float2 border = texelSize * borderClamp;

    // for parallax modify
    float4 n0 = impostorNormalAtlas.SampleLevel(impostorSampler, vp0uv, 1);
    float4 n1 = impostorNormalAtlas.SampleLevel(impostorSampler, vp1uv, 1);
    float4 n2 = impostorNormalAtlas.SampleLevel(impostorSampler, vp2uv, 1);

    float n0s = 0.5f - n0.a;
    float n1s = 0.5f - n1.a;
    float n2s = 0.5f - n2.a;

    float2 n0p = imp.frame0.zw * n0s;
    float2 n1p = imp.frame1.zw * n1s;
    float2 n2p = imp.frame2.zw * n2s;

    // add parallax shift
    vp0uv += n0p;
    vp1uv += n1p;
    vp2uv += n2p;

    // clamp out neighboring frames
    vp0uv = clamp(vp0uv, frame0 + border, frame0 + gridSize - border);
    vp1uv = clamp(vp1uv, frame1 + border, frame1 + gridSize - border);
    vp2uv = clamp(vp2uv, frame2 + border, frame2 + gridSize - border);

    float2 ddxy = float2(ddx(imp.uv.x), ddy(imp.uv.y));

    worldNormal = ImpostorBlendWeights(impostorNormalAtlas, impostorSampler, imp.uv, vp0uv, vp1uv, vp2uv, weights, ddxy);
    baseTex = ImpostorBlendWeights(impostorBaseAtlas, impostorSampler, imp.uv, vp0uv, vp1uv, vp2uv, weights, ddxy);
}

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
    //float depth : SV_DEPTH;
};

PS_OUTPUT main(VS_OUT input)
{
    PS_OUTPUT output;

    ImpostorData imp;
    imp.uv = input.texcoord.xy;
    imp.grid = input.texcoord.zw;
    imp.frame0 = input.plane0;
    imp.frame1 = input.plane1;
    imp.frame2 = input.plane2;

    float4 baseTex;
    float4 normalTex;

    ImpostorSample(imp, baseTex, normalTex);
    baseTex.a = saturate(pow(baseTex.a, Cutoff));
    //clip(baseTex.a - Cutoff);

    // scale world normal back to -1 to 1
    float3 worldNormal = normalTex.xyz * 2.0f - 1.0f;

    worldNormal = mul(WorldMatrix, float4(worldNormal, 0.0f)).xyz;

    //output.depth = normalTex.w;

    //float3 t = input.tangentWorld;
    //float3 b = input.bitangentWorld;
    //float3 n = input.normalWorld;

    //float3x3 tangentToWorld = float3x3(t, b, n);

    //output.color = float4(baseTex.rgb, 1.0f);
    output.color = float4(baseTex.rgb, 1.0f);
    return output;
}