import Common.Lighting;
import Common.RenderScene;

struct GBufferTextures
{
    Sampler2D positionTexture;
    Sampler2D normalTexture;
    Sampler2D albedoTexture;
}

struct FSOutput
{
    float4 Color : SV_Target0;
}

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
}

[[vk::binding(0, 0)]]
ConstantBuffer<View> view;

[[vk::binding(1, 0)]]
ConstantBuffer<DirectionalLight> directionalLight;
[[vk::binding(2, 0)]]
ConstantBuffer<PointLight[kMaxPointLight]> pointLights;
[[vk::binding(3, 0)]]
ConstantBuffer<uint> pointLightCount;

[[vk::binding(0, 1)]]
GBufferTextures gBufferTextures;

[shader("vertex")]
VSOutput VSMain(uint vertexID: SV_VertexID)
{
    VSOutput output;
    output.UV = float2((vertexID << 1) & 2, vertexID & 2);
    output.Position = float4(output.UV * 2.0f - 1.0f, 0.0f, 1.0f);
    return output;
}

[shader("fragment")]
FSOutput FSMain(VSOutput input)
{
    FSOutput output;

    float3 position = gBufferTextures.positionTexture.Sample(input.UV).rgb;
    float3 normal = gBufferTextures.normalTexture.Sample(input.UV).rgb;
    float3 albedo = gBufferTextures.albedoTexture.Sample(input.UV).rgb;

    float3 result = float3(0.0f, 0.0f, 0.0f);

    float3 viewDirection = normalize(view.Position - position);

    float fixedDistance = 1.0f;
    float epsilon = 1.0e-5f;

    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    ComputedDirectionalLight computedDirectionalLight = ComputeDirectionalLight(directionalLight, normal);
    result += computedDirectionalLight.Result * albedo;

    for (uint i = 0; i < pointLightCount; i++)
    {
        ComputedPointLight computedLight = ComputePointLight(pointLights[i], normal, position);
        float attenation = InverseSquareLightAttenuation(computedLight.Distance, fixedDistance, epsilon);
        result += computedLight.Result * albedo * attenation;
    }

    output.Color = float4(result, 1.0f);

    return output;
}
