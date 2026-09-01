#include "Shared/Defines.slang"
#include "Shared/DefaultRender.slang"
#include "Shared/Lighting.slang"

WLSL_BINDING(0, 0)
ConstantBuffer<ViewData> View;

WLSL_BINDING(1, 0)
ConstantBuffer<PointLightData> Light;

WLSL_BINDING(0, 1)
StructuredBuffer<RenderInstanceData> RenderInstanceBuffer;

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
    float4 FragColor : SV_TARGET;
};

[shader("vertex")]
VSOutput VSMain(VSInput input, uint InstanceIndex: SV_InstanceID)
{
    VSOutput output;

    RenderInstanceData instance = RenderInstanceBuffer[InstanceIndex];

    float4 worldPosition = mul(instance.Model, float4(input.Position, 1.0));

    float3x3 worldMatrix = (float3x3)instance.Model;

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

    RenderInstanceData instance = RenderInstanceBuffer[input.InstanceIndex];
    MaterialData material = Materials[instance.MaterialIndex];

    Texture2D diffuseTexture = Texture2DRegistry[NonUniformResourceIndex(material.DiffuseIndex)];
    float4 diffuseColor = diffuseTexture.Sample(TextureSampler, input.UV);
    diffuseColor *= material.DiffuseFactor;

    float3 normal = normalize(input.Normal);

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

    float3 viewDirection = normalize(View.Position - input.WorldPosition);
    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    float3 lightColor = Light.Color;
    float3 lightDirection = Light.Position - input.WorldPosition;
    float lightDistance = length(lightDirection);
    lightDirection = normalize(lightDirection);

    float lambertian = max(dot(lightDirection, normal), 0.0f);
    float3 result = diffuseColor.rgb * lambertian + diffuseColor.rgb * 0.1f;

    float epsilon = 1.0e-5f;
    result *= InverseSquareLightAttenuation(lightDistance, 1.0f, epsilon);

    output.FragColor.rgb = result;
    output.FragColor.a = diffuseColor.a;

    return output;
}
