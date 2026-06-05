
#include "Shared/Defines.hlsli"

WLS_BINDING(0, 0)  
cbuffer ViewBuffer : register(b0, space0)
{
    ViewInstance View;
}

WLS_BINDING(1, 0)
cbuffer PunctualLightBuffer : register(b1, space0)
{
    PunctualLight PunctualLight;
}

WLS_BINDING(0, 1)
RWStructuredBuffer<RenderInstance> RenderInstanceBuffer : register(u0, space1);

WLS_BINDING(0, 2)
Texture2D Texture2DRegistry[] : register(t0, space2);
WLS_BINDING(0, 2)
SamplerState Samplers : register(s0, space2);

WLS_BINDING(0, 3) 
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
    WLS_LOCATION(0)
    float4 FragColor : SV_TARGET;
};

float InverseSquareLightAttenuation(float lightDistance, float fixedDistance, float epsilon)
{
    return (fixedDistance * fixedDistance) / ((lightDistance * lightDistance) + epsilon);
}

FSOutput main(FSInput input)
{
    FSOutput output;
    
    RenderInstance instance = RenderInstanceBuffer[input.InstanceIndex];
    Material material = Materials[instance.MaterialIndex];
    
    Texture2D diffuseTexture = Texture2DRegistry[NonUniformResourceIndex(material.DiffuseIndex)];
    float4 diffuseColor = diffuseTexture.Sample(Samplers, input.UV);
    diffuseColor *= material.DiffuseFactor;
    
    float3 normal = normalize(input.Normal);
    
    if (material.NormalIndex != WLS_INVALID_TEXTURE_INDEX)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));
        
        Texture2D normalTexture = Texture2DRegistry[NonUniformResourceIndex(material.NormalIndex)];
        normal = normalTexture.Sample(Samplers, input.UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(mul(TBN, normal));
    }
    
    float3 viewDirection = normalize(View.EyePosition - input.Position);
    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }
    
    float3 lightColor = PunctualLight.Color;
    float3 lightDirection = PunctualLight.Position - input.Position;
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
