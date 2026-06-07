#include "Shared/Defines.hlsli"
#include "Shared/DefaultRender.hlsli"
#include "Shared/Lighting.hlsli"

WLSL_BINDING(0, 0)  
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLSL_BINDING(1, 0)
cbuffer PunctualLightBuffer : register(b1, space0)
{
    PunctualLight Light;
}

WLSL_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
WLSL_BINDING(0, 2)
SamplerState Samplers : register(s0, space2);

WLSL_BINDING(0, 3) 
RWStructuredBuffer<Material> Materials : register(u0, space3);

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

struct FSOutput
{
    WLSL_LOCATION(0)
    float4 FragColor : SV_TARGET;
};

VSOutput VSMain(VSInput input, uint InstanceIndex : SV_InstanceID)
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

FSOutput FSMain(VSOutput input)
{
    FSOutput output;
    
    RenderInstance instance = RenderInstanceBuffer[input.InstanceIndex];
    Material material = Materials[instance.MaterialIndex];
    
    Texture2D diffuseTexture = Texture2DRegistry[NonUniformResourceIndex(material.DiffuseIndex)];
    float4 diffuseColor = diffuseTexture.Sample(Samplers, input.UV);
    diffuseColor *= material.DiffuseFactor;
    
    float3 normal = normalize(input.Normal);
    
    if (material.NormalIndex != WLSL_INVALID_TEXTURE_INDEX)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));
        
        Texture2D normalTexture = Texture2DRegistry[NonUniformResourceIndex(material.NormalIndex)];
        normal = normalTexture.Sample(Samplers, input.UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(mul(TBN, normal));
    }
    
    float3 viewDirection = normalize(View.EyePosition - input.WorldPosition);
    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }
    
    float3 lightColor = Light.Color;
    float3 lightDirection = Light.Position - input.WorldPosition;
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);
    
    float lambertian = max(dot(lightDirection , normal), 0.0f);
    float3 result = diffuseColor.rgb * lambertian + diffuseColor.rgb * 0.1f;
    
    float epsilon = 1.0e-5f;
    result *= InverseSquareLightAttenuation(lightDistance, 1.0f, epsilon);
    
    output.FragColor.rgb = result;
    output.FragColor.a = diffuseColor.a;
    
    return output;
}
