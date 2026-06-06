
#include "Shared/Defines.hlsli"

WLSL_BINDING(0, 0)
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
SamplerState Samplers : register(s0, space2);

WLSL_BINDING(0, 3)
RWStructuredBuffer<Material> Materials : register(u0, space3);

struct FSInput
{
    float4 Pos : SV_POSITION;
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : TEXCOORD1;
    uint InstanceIndex : SV_INSTANCEID;
};

struct FSOutput
{
    WLSL_LOCATION(0)
    float4 Position : SV_TARGET0;
    
    WLSL_LOCATION(1)
    float4 Normal : SV_TARGET1;
    
    WLSL_LOCATION(2)
    float4 Albedo : SV_TARGET2;
};

FSOutput main(FSInput input)
{
    FSOutput output;
   
    RenderInstance instance = RenderInstanceBuffer[input.InstanceIndex];
    Material material = Materials[instance.MaterialIndex];
    
    output.Position = float4(input.Position, 1.0f);
    
    float3 normal = input.Normal;
    if (material.NormalIndex != WLSL_INVALID_TEXTURE_INDEX)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));
    
        Texture2D normalTexture = Texture2DRegistry[NonUniformResourceIndex(material.NormalIndex)];
        normal = normalTexture.Sample(Samplers, input.UV).rgb;
        normal = normalize(mul(TBN, normal));
    }
    output.Normal = float4(normal, 0.0f);
    
    if (material.DiffuseIndex != WLSL_INVALID_TEXTURE_INDEX)
    {
        Texture2D diffuseTexture = Texture2DRegistry[NonUniformResourceIndex(material.DiffuseIndex)];
        output.Albedo = diffuseTexture.Sample(Samplers, input.UV) * material.DiffuseFactor;
    }
    else
    {
        output.Albedo = material.DiffuseFactor;
    }
    
    
    return output;
}
