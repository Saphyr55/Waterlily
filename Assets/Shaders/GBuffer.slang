#include "Shared/Defines.slang"
#include "Shared/DefaultRender.slang"

WLSL_BINDING(0, 0)
ConstantBuffer<ViewData> View;

WLSL_BINDING(0, 1)
StructuredBuffer<RenderInstanceData> RenderInstance;

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[];
SamplerState TextureSampler;

WLSL_BINDING(0, 3)
StructuredBuffer<MaterialData> Materials;

struct VSInput
{
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float4 Tangent : TANGENT0;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float3 WorldPosition : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : TEXCOORD1;
    uint InstanceIndex : SV_InstanceID;
};

struct FSOutput
{
    float4 Position : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Albedo : SV_TARGET2;
};

[shader("vertex")]
VSOutput VSMain(VSInput input, uint InstanceIndex : SV_InstanceID)
{
    VSOutput output;
    
    RenderInstanceData instance = RenderInstance[InstanceIndex];
    
    float4 worldPosition = mul(instance.Model, float4(input.Position, 1.0));

    float3x3 worldMatrix = (float3x3) instance.Model;

    float3 normal = normalize(mul(worldMatrix, input.Normal));
    float3 tangent = normalize(mul(worldMatrix, input.Tangent.xyz));
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

[shader("fragment")]
FSOutput FSMain(VSOutput input)
{
    FSOutput output;

    RenderInstanceData instance = RenderInstance[input.InstanceIndex];
    MaterialData material = Materials[instance.MaterialIndex];
    
    output.Position = float4(input.WorldPosition, 1.0f);
    
    float3 normal = input.Normal;
    if (material.NormalIndex != WLSL_INVALID_TEXTURE_INDEX)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

        Texture2D normalTexture = Texture2DRegistry[NonUniformResourceIndex(material.NormalIndex)];
        normal = normalTexture.Sample(TextureSampler, input.UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(mul(TBN, normal));
    }
    output.Normal = float4(normal, 0.0f);
    
    if (material.DiffuseIndex != WLSL_INVALID_TEXTURE_INDEX)
    {
        Texture2D diffuseTexture = Texture2DRegistry[NonUniformResourceIndex(material.DiffuseIndex)];
        output.Albedo = diffuseTexture.Sample(TextureSampler, input.UV) * material.DiffuseFactor;
    }
    else
    {
        output.Albedo = material.DiffuseFactor;
    }
    
    return output;
}
