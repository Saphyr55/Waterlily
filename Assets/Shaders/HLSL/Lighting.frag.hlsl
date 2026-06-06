#include "Shared/Defines.hlsli"

WLSL_BINDING(0, 0)  
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLSL_BINDING(1, 0)
cbuffer PunctualLightBuffer : register(b1, space0)
{
    PunctualLight PunctualLight;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
WLSL_BINDING(0, 2)
SamplerState Samplers : register(s0, space2);

WLSL_BINDING(0, 3) 
RWStructuredBuffer<Material> Materials : register(u0, space3);

WLSL_BINDING(0, 4)
Texture2D PositionTexture : register(t0, space4);

WLSL_BINDING(1, 4)
Texture2D NormalTexture : register(t1, space4);

WLSL_BINDING(2, 4) 
Texture2D AlbedoTexture : register(t2, space4);

struct FSInput
{
    float4 Position : SV_Position;
};

struct FSOutput
{
    WLSL_LOCATION(0)
    float4 Color : SV_Target0;
};

FSOutput main(FSInput input)
{
    FSOutput output;

    int2 Coord = int2(input.Position.xy);
    int3 CoordFetch = int3(Coord, 0);

    float3 position = PositionTexture.Load(CoordFetch).rgb;
    float3 albedo = AlbedoTexture.Load(CoordFetch).rgb;
    float3 normal = normalize(NormalTexture.Load(CoordFetch).rgb * 2.0f - 1.0f);
    float3 lightDirection = normalize(float3(0.5f, -1.0f, 0.5f));

    float NdotL = saturate(dot(normal, -lightDirection));

    float3 color = albedo * NdotL;
    
    output.Color = float4(color, 1.0f);

    return output;
}
