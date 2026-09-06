import Common.Light;
import Common.Scene;
import Common.Math;
import PBR.PBR;
import PBR.Material;

using Light;
using PBR;

struct GBufferTextures
{
    Sampler2D positionTexture;
    Sampler2D normalTexture;
    Sampler2D albedoTexture;
    Sampler2D metallicRoughnessTexture;
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
    float4 albedo = gBufferTextures.albedoTexture.Sample(input.UV);
    float3 metallicRoughness = gBufferTextures.metallicRoughnessTexture.Sample(input.UV).rgb;

    float metallic = metallicRoughness.x;
    float roughness = metallicRoughness.y;

    float3 viewDirection = normalize(view.position - position);

    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    float3 F0 = float3(0.04, 0.04, 0.04);
    F0 = lerp(F0, albedo.rgb, metallic);

    MaterialBRDF materialBRDF;
    materialBRDF.albedo = albedo.rgb;
    materialBRDF.metallic = metallic;
    materialBRDF.roughness = roughness;
    materialBRDF.F0 = F0;

    float3 Lo = float3(0.0f, 0.0f, 0.0f);

    DirectionalLight dl = directionalLight;
    dl.color = Lo;
    Lo += PBR.CookTorranceBRDF(dl, materialBRDF, normal, viewDirection, position);

    for (uint i = 0; i < pointLightCount; i++)
    {
        PointLight pointLight = pointLights[i];
        Lo += PBR.CookTorranceBRDF(pointLight, materialBRDF, normal, viewDirection, position);
    }

    float3 color = Lo;
    color = color / (color + float3(1.0, 1.0, 1.0));

    output.Color = float4(color, albedo.a);

    return output;
}
