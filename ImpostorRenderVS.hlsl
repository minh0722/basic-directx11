#include "ImpostorShaderCommon.cginc"

cbuffer WorldViewProj : register(b0)
{
    matrix world;
    matrix view;
    matrix proj;
};

cbuffer VertexConstants : register(b1)
{
    matrix worldToObject;
    float4 cameraWorldPos;
    float framesCount;
    float radius;
};

ByteAddressBuffer VertexDataBuffer : register(t0);

void ImpostorVertex(inout ImpostorData imp)
{
    float4 vertex = imp.vertex;
    float2 texcoord = imp.uv;

    float3 impostorPivotOffset = float3(0.0f, 0.0f, 0.0f);      // at 0,0,0 for now
    float framesMinusOne = framesCount - 1;

    float3 cameraPosObjectSpace = mul(cameraWorldPos, worldToObject).xyz;
    float3 pivotToCameraRay = normalize(cameraPosObjectSpace - impostorPivotOffset);
    
    // scale uv to single frame
    texcoord *= (1.0f / framesCount);

    float2 size = float(radius * 2.0f).xx;

    float3 projected = SpriteProjection(pivotToCameraRay, framesCount, size, texcoord);

    // this creates the proper offset for vertices to camera facing billboard
    float3 vertexOffset = projected + impostorPivotOffset;
    vertexOffset = normalize(cameraPosObjectSpace - vertexOffset);
    vertexOffset += projected;
    vertexOffset -= vertex.xyz;
    vertexOffset += impostorPivotOffset;

    // camera to projection vector
    float3 rayDirectionLocal = (impostorPivotOffset + projected) - cameraPosObjectSpace;

    // projected position to camera ray
    float3 projInterpolated = normalize(cameraPosObjectSpace - (projected + impostorPivotOffset));

    Ray rayLocal;
    rayLocal.origin = cameraPosObjectSpace - impostorPivotOffset;
    rayLocal.direction = rayDirectionLocal;

    float2 grid = VectorToGrid(pivotToCameraRay);
    grid = saturate((grid + 1.0f) * 0.5f);      // bias and scale to 0 to 1. This is reverse to what FrameXYToRay does
    grid *= framesMinusOne;

    float2 gridFrac = frac(grid);
    float2 gridFloor = floor(grid);

    float4 weights = TriangleInterpolate(gridFrac);

    // 3 nearest frames
    float2 frame0 = gridFloor;
    float2 frame1 = gridFloor + lerp(float2(0.0f, 1.0f), float2(1.0f, 0.0f), weights.w);
    float2 frame2 = gridFloor + float2(1.0f, 1.0f);

    // convert frame coordinate to octahedron direction
    float3 frame0Ray = FrameXYToRay(frame0, framesMinusOne.xx);
    float3 frame1Ray = FrameXYToRay(frame1, framesMinusOne.xx);
    float3 frame2Ray = FrameXYToRay(frame2, framesMinusOne.xx);

    float3 planeCenter = float3(0.0f, 0.0f, 0.0f);

    // per frame uv
    float3 plane0X;
    float3 plane0Normal = frame0Ray;
    float3 plane0Z;
    float3 frame0Local = FrameTransform(projInterpolated, frame0Ray, plane0X, plane0Z);
    frame0Local.xz = frame0Local.xz / framesCount.xx;   // for displacement

    float2 virtualUV0 = VirtualPlaneUV(plane0Normal, plane0X, plane0Z, planeCenter, size, rayLocal);
    virtualUV0 /= framesCount.xx;

    // per frame uv
    float3 plane1X;
    float3 plane1Normal = frame1Ray;
    float3 plane1Z;
    float3 frame1Local = FrameTransform(projInterpolated, frame1Ray, plane1X, plane1Z);
    frame1Local.xz = frame1Local.xz / framesCount.xx;   // for displacement

    float2 virtualUV1 = VirtualPlaneUV(plane1Normal, plane1X, plane1Z, planeCenter, size, rayLocal);
    virtualUV1 /= framesCount.xx;

    // per frame uv
    float3 plane2X;
    float3 plane2Normal = frame2Ray;
    float3 plane2Z;
    float3 frame2Local = FrameTransform(projInterpolated, frame2Ray, plane2X, plane2Z);
    frame2Local.xz = frame2Local.xz / framesCount.xx;   // for displacement

    float2 virtualUV2 = VirtualPlaneUV(plane2Normal, plane2X, plane2Z, planeCenter, size, rayLocal);
    virtualUV2 /= framesCount.xx;

    imp.vertex.xyz += vertexOffset;
    imp.uv = texcoord;
    imp.grid = grid;
    imp.frame0 = float4(virtualUV0, frame0Local.xz);
    imp.frame1 = float4(virtualUV1, frame1Local.xz);
    imp.frame2 = float4(virtualUV2, frame2Local.xz);
}

VS_OUT main(uint vertexID : SV_VertexID)
{
    float3 vertex = VertexIDToQuadVertex(vertexID);
    uint perVertexSize = 8;    //2 float uv
    float2 uv = asfloat(VertexDataBuffer.Load2(vertexID * perVertexSize));

    ImpostorData imp = (ImpostorData)0;
    imp.vertex = float4(vertex, 1.0f);
    imp.uv = uv;

    ImpostorVertex(imp);

    float4 normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
    float4 tangent = float4(1.0f, 0.0f, 0.0f, 1.0f);

    float3 normalWorld = normalize(mul((float3x3)world, normal.xyz));
    float3 tangentWorld = normalize(mul((float3x3)world, tangent.xyz));
    float3 bitangentWorld = cross(normalWorld, tangentWorld);

    VS_OUT o;
    o.vertex = mul(world, imp.vertex);
    o.vertex = mul(view, o.vertex);
    o.vertex = mul(proj, o.vertex);

    o.tangentWorld = tangentWorld;
    o.bitangentWorld = bitangentWorld;
    o.normalWorld = normalWorld;
    o.texcoord.xy = imp.uv;
    o.texcoord.zw = imp.grid;
    o.plane0 = imp.frame0;
    o.plane1 = imp.frame1;
    o.plane2 = imp.frame2;

    float3 worldPos = mul(world, vertex).xyz;
    o.tangentSpace0 = float4(tangentWorld.x, bitangentWorld.x, normalWorld.x, worldPos.x);
    o.tangentSpace1 = float4(tangentWorld.y, bitangentWorld.y, normalWorld.y, worldPos.y);
    o.tangentSpace2 = float4(tangentWorld.z, bitangentWorld.z, normalWorld.z, worldPos.z);

    return o;
}