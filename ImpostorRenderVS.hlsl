
cbuffer WorldViewProj : register(b0)
{
    matrix worldViewProj;
};

cbuffer VertexConstants : register(b1)
{
    matrix worldToObject;
    float4 cameraWorldPos;
    float framesCount;
    float radius;
};

ByteAddressBuffer VertexDataBuffer : register(t0);

struct Ray
{
    float3 origin;
    float3 direction;
};

struct ImpostorData
{
    float2 uv;
    float2 grid;
    float4 frame0;
    float4 frame1;
    float4 frame2;
    float3 vertex;
};

static const bool g_ImposterFullSphere = true;

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

float2 VirtualPlaneUV(float3 planeNormal, float3 planeX, float3 planeZ, float3 center, float2 uvScale, Ray rayLocal)
{
    half normalDotOrigin = dot(planeNormal, rayLocal.origin);
    half normalDotCenter = dot(planeNormal, center);
    half normalDotRay = dot(planeNormal, rayLocal.direction);

    half planeDistance = normalDotOrigin - normalDotCenter;
    planeDistance *= -1.0f;

    half intersect = planeDistance / normalDotRay;

    float3 intersection = ((rayLocal.direction * intersect) + rayLocal.origin) - center;

    half dx = dot(planeX, intersection);
    half dz = dot(planeZ, intersection);

    float2 uv = float2(0.0f, 0.0f);

    if (intersect > 0.0f)
    {
        uv = float2(dx, dz);
    }
    else
    {
        uv = float2(0.0f, 0.0f);
    }

    uv /= uvScale;
    uv += float2(0.5f, 0.5f);
    return uv;
}

float3 ITBasis(float3 vec, float3 basedX, float3 basedY, float3 basedZ)
{
    return float3(dot(basedX, vec), dot(basedY, vec), dot(basedZ, vec));
}

float3 FrameTransform(float3 projRay, float3 frameRay, out float3 worldX, out float3 worldZ)
{
    worldX = normalize(float3(-frameRay.z, 0, frameRay.x));
    worldZ = normalize(cross(worldX, frameRay));

    projRay *= -1.0f;

    float3 local = normalize(ITBasis(projRay, worldX, frameRay, worldZ));
    return local;
}

float4 TriangleInterpolate(float2 uv)
{
    uv = frac(uv);

    float2 omuv = float2(1.0, 1.0) - uv.xy;

    float4 res = float4(0, 0, 0, 0);
    //frame 0
    res.x = min(omuv.x, omuv.y);
    //frame 1
    res.y = abs(dot(uv, float2(1.0, -1.0)));
    //frame 2
    res.z = min(uv.x, uv.y);
    //mask
    res.w = saturate(ceil(uv.x - uv.y));

    return res;
}

float3 OctaHemiEnc(float2 coord)
{
    coord = float2(coord.x + coord.y, coord.x - coord.y) * 0.5f;
    float3 vec = float3(coord.x, 1.0f - dot(float2(1.0f, 1.0f), abs(coord)), coord.y);
    return vec;
}

float3 OctaSphereEnc(float2 coord)
{
    float3 vec = float3(coord.x, 1.0f - dot(1.0f, abs(coord)), coord.y);
    if (vec.y < 0.0f)
    {
        float2 flip = vec.xz >= 0.0f ? float2(1.0f, 1.0f) : float2(-1.0f, -1.0f);
        vec.xz = (1.0f - abs(vec.zx)) * flip;
    }
    return vec;
}

float3 GridToVector(float2 coord)
{
    float3 vec;
    if (g_ImposterFullSphere)
    {
        vec = OctaSphereEnc(coord);
    }
    else
    {
        vec = OctaHemiEnc(coord);
    }
    return vec;
}

float3 FrameXYToRay(float2 frame, float2 frameCountMinusOne)
{
    //divide frame x y by framecount minus one to get 0-1
    float2 f = frame.xy / frameCountMinusOne;
    //bias and scale to -1 to 1
    f = (f - 0.5f) * 2.0f;
    //convert to vector, either full sphere or hemi sphere
    float3 vec = GridToVector(f);
    vec = normalize(vec);
    return vec;
}


float2 VecToHemiOct(float3 vec)
{
    vec.xz /= dot(1.0f, abs(vec));
    return float2(vec.x + vec.z, vec.x - vec.z);
}

float2 VecToSphereOct(float3 vec)
{
    vec.xz /= dot(1.0f, abs(vec));
    if (vec.y <= 0.0f)
    {
        float2 flip = vec.xz >= 0.0f ? float2(1.0f, 1.0f) : float2(-1.0f, -1.0f);
        vec.xz = (1.0f - abs(vec.zx)) * flip;
    }
    return vec.xz;
}

float2 VectorToGrid(float3 vec)
{
    float2 coord;

    if (g_ImposterFullSphere)
    {
        coord = VecToSphereOct(vec);
    }
    else
    {
        vec.y = max(0.001f, vec.y);
        vec = normalize(vec);
        coord = VecToHemiOct(vec);
    }
    return coord;
}

float3 SpriteProjection(float3 pivotToCameraRayLocal, float framesCount, float2 size, float2 coord)
{
    float3 gridVec = pivotToCameraRayLocal;

    // octahedron vector, pivot to camera
    float3 y = normalize(gridVec);

    float3 x = normalize(cross(y, float3(0.0f, 1.0f, 0.0f)));
    float3 z = normalize(cross(x, y));

    float2 uv = ((coord * framesCount) - 0.5f) * 2.0f;   // -1 to 1

    float3 newX = x * uv.x;
    float3 newZ = z * uv.y;

    float2 halfSize = size * 0.5f;

    newX *= halfSize.x;
    newZ *= halfSize.y;

    float3 res = newX + newZ;

    return res;
}

void ImpostorVertex(inout ImpostorData imp)
{
    float3 vertex = imp.vertex;
    float2 texcoord = imp.uv;

    float3 impostorPivotOffset = float3(0.0f, 0.0f, 0.0f);      // at 0,0,0 for now
    float framesMinusOne = framesCount - 1;

    float3 cameraPosObjectSpace = mul(worldToObject, cameraWorldPos).xyz;
    float3 pivotToCameraRay = normalize(cameraPosObjectSpace - impostorPivotOffset);
    
    // scale uv to single frame
    texcoord *= (1.0f / framesCount);

    float2 size = float(radius * 2.0f).xx;

    float3 projected = SpriteProjection(pivotToCameraRay, framesCount, size, texcoord);

    // this creates the proper offset for vertices to camera facing billboard
    float3 vertexOffset = projected + impostorPivotOffset;
    vertexOffset = normalize(cameraPosObjectSpace - vertexOffset);
    vertexOffset += projected;
    vertexOffset -= vertex;
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

    imp.vertex += vertexOffset;
    imp.uv = texcoord;
    imp.grid = grid;
    imp.frame0 = float4(virtualUV0, frame0Local.xz);
    imp.frame1 = float4(virtualUV1, frame1Local.xz);
    imp.frame2 = float4(virtualUV2, frame2Local.xz);
}

void main(uint vertexID : SV_VertexID)
{
    float3 vertex = VertexIDToQuadVertex(vertexID);
    float2 uv = VertexDataBuffer.Load2(vertexID * 2 * 4);   // 2 float uv

    ImpostorData imp = (ImpostorData)0;
    imp.vertex = vertex;
    imp.uv = uv;

    ImpostorVertex(imp);

    float4 normal = float4(0.0f, 1.0f, 0.0f, 1.0f);
    float4 tangent = float4(1.0f, 0.0f, 0.0f, 1.0f);


}