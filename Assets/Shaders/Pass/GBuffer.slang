import Common.Light;
import Common.Scene;
import PBR.Material;

struct VSInput
{
    float3 Position : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float4 Tangent : TANGENT0;
}

struct VSOutput
{
    float4 Position : SV_Position;
    float3 WorldPosition : POSITION0;
    float3 Normal : NORMAL0;
    float2 UV : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : TEXCOORD1;
    nointerpolation uint InstanceIndex : SV_InstanceID;
}

struct FSOutput
{
    float4 Position : SV_Target0;
    float4 Normal : SV_Target1;
    float4 Albedo : SV_Target2;
    float4 MetallicRoughness : SV_Target3;
}

[[vk::binding(0, 0)]]
ConstantBuffer<View> view;

[[vk::binding(0, 1)]]
StructuredBuffer<RenderInstance> renderInstanceBuffer;

[[vk::binding(0, 2)]]
Sampler2D[] samplers2D;

[[vk::binding(0, 3)]]
StructuredBuffer<Material> materialRegistry;

[shader("vertex")]
VSOutput VSMain(VSInput input, uint instanceIndex: SV_StartInstanceLocation)
{
    RenderInstance instance = renderInstanceBuffer[instanceIndex];

    float4 worldPosition = mul(instance.model, float4(input.Position, 1.0));

    float3x3 worldMatrix = float3x3(instance.model);

    float3 normal = normalize(mul(worldMatrix, input.Normal));
    float3 tangent = normalize(mul(worldMatrix, input.Tangent.xyz));
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent) * input.Tangent.w;

    VSOutput output;

    output.Position = mul(view.viewProj, worldPosition);
    output.WorldPosition = worldPosition.xyz;
    output.Normal = normal;
    output.UV = input.UV;
    output.Tangent = tangent;
    output.Bitangent = bitangent;
    output.InstanceIndex = instanceIndex;

    return output;
}

[shader("fragment")]
FSOutput FSMain(VSOutput input)
{
    FSOutput output;

    RenderInstance instance = renderInstanceBuffer.Load(input.InstanceIndex);
    Material material = materialRegistry.Load(instance.materialIndex);

    output.Position = float4(input.WorldPosition, 1.0f);

    if (material.baseColor != kInvalidTextureIndex)
    {
        Sampler2D diffuseTexture = samplers2D[NonUniformResourceIndex(material.baseColor)];
        output.Albedo = diffuseTexture.Sample(input.UV) * material.baseColorFactor;
    }
    else
    {
        output.Albedo = material.baseColor;
    }

    float3 normal = input.Normal;
    if (material.normal != kInvalidTextureIndex)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

        Sampler2D normalTexture = samplers2D[NonUniformResourceIndex(material.normal)];
        normal = normalTexture.Sample(input.UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(mul(TBN, normal));
    }
    output.Normal = float4(normal, 0.0f);

    float4 metallicRoughness = float4(material.metallicFactor, material.roughnessFactor, 0.0, 1.0);
    if (material.metallicRoughness != kInvalidTextureIndex)
    {
        Sampler2D metallicRoughnessSampler = samplers2D[NonUniformResourceIndex(material.metallicRoughness)];
        metallicRoughness = metallicRoughnessSampler.Sample(input.UV);
        metallicRoughness.x *= material.metallicFactor;
        metallicRoughness.y *= material.roughnessFactor;    
    }
    output.MetallicRoughness = metallicRoughness;

    return output;
}
