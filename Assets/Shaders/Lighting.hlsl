#include "Shared/Defines.hlsli"
#include "Shared/DefaultRender.hlsli"
#include "Shared/Lighting.hlsli"

#define WLSL_MAX_LIGHTS 32

WLSL_BINDING(0, 0)  
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLSL_BINDING(1, 0)
cbuffer PunctualLightBuffer : register(b1, space0)
{
    PunctualLight PunctualLights[WLSL_MAX_LIGHTS];
}

WLSL_BINDING(2, 0)
cbuffer BufferCounters : register(b2, space0)
{
    uint PunctualLightCount;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
WLSL_BINDING(0, 2)
SamplerState Texture2DRegistrySampler : register(s0, space2);

WLSL_BINDING(0, 3) 
RWStructuredBuffer<Material> Materials : register(u0, space3);

WLSL_BINDING(0, 4)
Texture2D PositionTexture : register(t0, space4);
SamplerState PositionSampler : register(s0, space4);

WLSL_BINDING(1, 4)
Texture2D NormalTexture : register(t1, space4);
SamplerState NormalSampler : register(s1, space4);

WLSL_BINDING(2, 4)
Texture2D AlbedoTexture : register(t2, space4);
SamplerState AlbedoSampler : register(s2, space4);


struct FSOutput
{
    WLSL_LOCATION(0)
    float4 Color : SV_Target0;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    output.UV = float2((vertexID << 1) & 2, vertexID & 2);
    output.Position = float4(output.UV * 2.0f - 1.0f, 0.0f, 1.0f);
    return output;
}

FSOutput FSMain(VSOutput input)
{
    FSOutput output;

    float3 position = PositionTexture.Sample(PositionSampler, input.UV).rgb;
    float3 normal = NormalTexture.Sample(NormalSampler, input.UV).rgb;
    float3 albedo = AlbedoTexture.Sample(AlbedoSampler, input.UV).rgb;

    float3 result = float3(0.0f, 0.0f, 0.0f);

    float3 eyeDirection = normalize(View.EyePosition - position);
    
    float fixedDistance = 1.0f;
    float epsilon = 1.0e-5f;

    for (uint i = 0; i < PunctualLightCount; i++)
    {
        ComputedPunctualLight computedLight = ComputePunctualLight(PunctualLights[i], normal, position, eyeDirection);
        float attenation = InverseSquareLightAttenuation(computedLight.Distance, fixedDistance, epsilon);
        result += computedLight.Result * albedo * attenation;
    }

    output.Color = float4(result, 1.0f);
    
    return output;
}
