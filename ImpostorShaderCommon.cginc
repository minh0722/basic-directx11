struct VS_OUT
{
    float4 vertex : SV_POSITION;
    float3 tangentWorld : TEXCOORD0;
    float3 bitangentWorld : TEXCOORD1;
    float3 normalWorld : TEXCOORD2;
    float4 texcoord : TEXCOORD3;
    float4 plane0 : TEXCOORD4;
    float4 plane1 : TEXCOORD5;
    float4 plane2 : TEXCOORD6;
    float4 tangentSpace0 : TEXCOORD7;
    float4 tangentSpace1 : TEXCOORD8;
    float4 tangentSpace2 : TEXCOORD9;
};

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
    float4 vertex;
};

static const bool g_ImposterFullSphere = true;

// @param vertexID in range [0; 3]
// returns a unit quad with following vertices <x, y, z>
// 0 - <-0.5f, 0, -0.5f>
// 1 - < 0.5f, 0, -0.5f>
// 2 - <-0.5f, 0,  0.5f>
// 3 - < 0.5f, 0,  0.5f>
float3 VertexIDToQuadVertex(uint vertexId)
{
    return float3(float((vertexId & 1)) - 0.5f, 0.0f, float((vertexId >> 1) & 1) - 0.5f);
}

float2 VirtualPlaneUV(float3 planeNormal, float3 planeX, float3 planeZ, float3 center, float2 uvScale, Ray rayLocal)
{
    half normalDotOrigin = dot(planeNormal, rayLocal.origin);
    half normalDotCenter = dot(planeNormal, center);
    half normalDotRay = dot(planeNormal, rayLocal.direction);

    half planeDistance = normalDotOrigin - normalDotCenter;
    planeDistance *= -1.0f;

    // ratio of camera to plane vector and camera to quad vertex vector
    half intersect = planeDistance / normalDotRay;

    // intersection of ray from camera to quad vertex with each of the view plane
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

// returns @vec in space of coord system with base vectors (basedX, basedY, basedZ)
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

    //uint2 frameUV = (coord * framesCount);  // single frame uv 0 to 1
    //uint idx = (frameUV.y << 1) | (frameUV.x << 0);
    //float2 uv = (float2(idx & 1, ~((idx & 2) >> 1) & 1) - 0.5f) * 2.0f;   // -1 to 1
    float2 uv = ((coord * framesCount) - 0.5f) * 2.0f;

    float3 newX = x * uv.x;
    float3 newZ = z * uv.y;

    float2 halfSize = size * 0.5f;

    newX *= halfSize.x;
    newZ *= halfSize.y;

    float3 res = newX + newZ;

    return res;
}