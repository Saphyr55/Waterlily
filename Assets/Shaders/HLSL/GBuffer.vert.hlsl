#include "Shared/Defines.hlsli"

WLSL_BINDING(0, 0)
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

struct VSInput
{
    WLSL_LOCATION(0)
    float3 Position : POSITION0;
    
    WLSL_LOCATION(1)
    float3 Normal : NORMAL0;
    
    WLSL_LOCATION(2) 
    float2 UV : TEXCOORD0;
    
    WLSL_LOCATION(3)
    float4 Tangent : TANGENT0;
};

struct VSOutput
{
    float4 Position : SV_POSITION;
    float3 WorldPosition : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : TEXCOORD1;
    uint InstanceIndex : SV_INSTANCEID;
};

VSOutput main(VSInput input, uint InstanceIndex : SV_InstanceID)
{
    VSOutput output;
    
    RenderInstance instance = RenderInstanceBuffer[InstanceIndex];
    
    float4 worldPosition = mul(instance.Model, float4(input.Position, 1.0));
    
    float3x3 worldMatrix = (float3x3) instance.Model;
    
    float3 normal = normalize(mul(worldMatrix, input.Normal));
    float3 tangent = normalize(mul(worldMatrix, (float3) input.Tangent));
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent) * input.Tangent.w;
    
    output.Position = mul(View.ViewProj, worldPosition);
    output.WorldPosition = worldPosition.xyz;
    output.Normal = normal;
    output.UV = input.UV;
    output.Tangent = tangent;
    output.Bitangent = bitangent;
    output.InstanceIndex = InstanceIndex;
    
    return output;
}

