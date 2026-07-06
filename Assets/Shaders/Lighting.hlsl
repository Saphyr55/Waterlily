#include "Shared/Defines.hlsli"
#include "Shared/DefaultRender.hlsli"
#include "Shared/Lighting.hlsli"

WLSL_BINDING(0, 0)  
cbuffer ViewCB : register(b0, space0)
{
    ViewData View;
}

WLSL_BINDING(1, 0)
cbuffer PointLightCB : register(b1, space0)
{
    PointLightData PointLights[WLSL_MAX_LIGHTS];
}

WLSL_BINDING(2, 0)
cbuffer DirectionalLightCB : register(b2, space0)
{
    DirectionalLightData DirectionalLight;
}

WLSL_BINDING(3, 0)
cbuffer CountersCB : register(b3, space0)
{
    uint PointLightCount;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstanceData> RenderInstanceRWSB : register(u0, space1);

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
WLSL_BINDING(0, 2)
SamplerState Texture2DRegistrySampler : register(s0, space2);

WLSL_BINDING(0, 3) 
RWStructuredBuffer<MaterialData> MaterialsRWSB : register(u0, space3);

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

    float3 viewDirection = normalize(View.Position - position);

    float fixedDistance = 1.0f;
    float epsilon = 1.0e-5f;

    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }   

    ComputedDirectionalLightData computedDirectionalLight = ComputeDirectionalLight(DirectionalLight, normal);
    result += computedDirectionalLight.Result * albedo;

    for (uint i = 0; i < PointLightCount; i++)
    {
        ComputedPointLightData computedLight = ComputePointLight(PointLights[i], normal, position);
        float attenation = InverseSquareLightAttenuation(computedLight.Distance, fixedDistance, epsilon);
        result += computedLight.Result * albedo * attenation;
    }

    output.Color = float4(result, 1.0f);
    
    return output;
}
