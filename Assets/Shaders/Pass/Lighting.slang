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

[vk::binding(0, 0)]
ConstantBuffer<View> view;

[vk::binding(1, 0)]
ConstantBuffer<DirectionalLight> directionalLight;
[vk::binding(2, 0)]
ConstantBuffer<PointLight[kMaxPointLight]> pointLights;
[vk::binding(3, 0)]
ConstantBuffer<uint> pointLightCount;

[vk::binding(0, 1)]
GBufferTextures gBufferTextures;
[vk::binding(4, 1)]
RWTexture2D outputColor;

[vk::binding(0, 2)]
Texture2D<float16_t> shadowMap;

[shader("compute")]
[numthreads(16, 16, 1)]
void Main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    uint2 pixel = dispatchThreadID.xy;

    float3 position = gBufferTextures.positionTexture[pixel].rgb;
    float3 normal = gBufferTextures.normalTexture[pixel].rgb;
    float4 albedo = gBufferTextures.albedoTexture[pixel];
    float3 metallicRoughness = gBufferTextures.metallicRoughnessTexture[pixel].rgb;

    float depth = shadowMap[pixel];
    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;

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
    dl.color = float3(0.0f, 0.0f, 0.0f);// Turn off directional light.
    Lo += PBR.CookTorranceBRDF(dl, materialBRDF, normal, viewDirection, position);

    for (uint i = 0; i < pointLightCount; i++)
    {
        Lo += PBR.CookTorranceBRDF(pointLights[i], materialBRDF, normal, viewDirection, position);
    }

    float3 color = Lo;
    color = color / (color + float3(1.0, 1.0, 1.0));

    color = color + float3(depth, depth, depth) * 0.00000001f;

    outputColor[pixel] = float4(color, albedo.a);
}
