import Common.Lighting;
import Common.RenderScene;
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
    nointerpolation uint InstanceIndex;
}

struct FSOutput
{
    float4 FragColor : SV_TARGET;
}

[[vk::binding(0, 0)]]
ConstantBuffer<View> view;

[[vk::binding(1, 0)]]
ConstantBuffer<PointLight[kMaxPointLight]> pointLights;

[[vk::binding(0, 1)]]
StructuredBuffer<RenderInstance> renderInstanceBuffer;

[[vk::binding(0, 2)]]
Sampler2D[] sampler2DRegistry;

[[vk::binding(0, 3)]]
StructuredBuffer<Material> materialRegistry;

[shader("vertex")]
VSOutput VSMain(VSInput input, uint instanceIndex: SV_StartInstanceLocation)
{
    VSOutput output;

    RenderInstance instance = renderInstanceBuffer[instanceIndex];

    float4 worldPosition = mul(instance.model, float4(input.Position, 1.0));

    float3x3 worldMatrix = float3x3(instance.model);

    float3 normal = normalize(mul(worldMatrix, input.Normal));
    float3 tangent = normalize(mul(worldMatrix, input.Tangent.xyz));
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    float3 bitangent = cross(normal, tangent) * input.Tangent.w;

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

    RenderInstance instance = renderInstanceBuffer[input.InstanceIndex];
    Material material = materialRegistry[instance.materialIndex];

    Sampler2D diffuseTexture = sampler2DRegistry[NonUniformResourceIndex(material.baseColor)];
    float4 diffuseColor = diffuseTexture.Sample(input.UV);
    diffuseColor *= material.baseColorFactor;

    float3 normal = normalize(input.Normal);

    if (material.normal != kInvalidTextureIndex)
    {
        float3 tangent = normalize(input.Tangent);
        float3 bitangent = normalize(input.Bitangent);
        float3x3 TBN = transpose(float3x3(tangent, bitangent, normal));

        Sampler2D normalTexture = sampler2DRegistry[NonUniformResourceIndex(material.normal)];
        normal = normalTexture.Sample(input.UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(mul(TBN, normal));
    }

    float3 viewDirection = normalize(view.position - input.WorldPosition);
    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    // TODO:
    PointLight light = pointLights[0];

    float3 lightColor = light.Color;
    float3 lightDirection = light.Position - input.WorldPosition;
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
