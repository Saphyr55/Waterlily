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

struct ShadowMappingInfo
{
    float4x4 lightSpace;
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
Sampler2D<float> shadowMap;
[vk::binding(1, 2)]
ConstantBuffer<View> directionalLightView;


float DirectionalLightComputeShadow(DirectionalLight light, float3 position, float3 normal)
{
    float4 lightSpacePos = mul(directionalLightView.viewProj, float4(position, 1.0f));
    float4 shadowCoord = lightSpacePos / lightSpacePos.w;
    shadowCoord.xy = shadowCoord.xy * 0.5f + 0.5f;
    
    uint width, height;
    shadowMap.GetDimensions(width, height);
    uint2 texel = uint2(shadowCoord.xy * float2(width, height));

    float currentDepth = shadowCoord.z;
    float bias = max(0.025f * (1.0f - dot(light.GetDirection(position), normal)), 0.005f);
    float closestDepth = shadowMap[texel];
    float shadow = currentDepth - bias > closestDepth ? 1.0f : 0.0f;

    return shadow;
}

[shader("compute")]
[numthreads(16, 16, 1)]
void Main(uint3 dispatchThreadID: SV_DispatchThreadID)
{
    uint2 pixel = dispatchThreadID.xy;

    float3 position = gBufferTextures.positionTexture[pixel].rgb;
    float3 normal = gBufferTextures.normalTexture[pixel].rgb;
    float4 albedo = gBufferTextures.albedoTexture[pixel];
    float3 metallicRoughness = gBufferTextures.metallicRoughnessTexture[pixel].rgb;

    float metallic = metallicRoughness.b;
    float roughness = metallicRoughness.g;

    float3 viewDirection = normalize(view.position - position);

    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    float shadow = DirectionalLightComputeShadow(directionalLight, position, normal);

    float3 F0 = float3(0.04f, 0.04f, 0.04f);
    F0 = lerp(F0, albedo.rgb, metallic);

    MaterialBRDF materialBRDF;
    materialBRDF.albedo = albedo.rgb;
    materialBRDF.metallic = metallic;
    materialBRDF.roughness = roughness;
    materialBRDF.F0 = F0;

    float3 Lo = PBR.CookTorranceBRDF(directionalLight, materialBRDF, normal, viewDirection, position) * (1.0f - shadow);
    for (uint i = 0; i < pointLightCount; i++)
    {
        Lo += PBR.CookTorranceBRDF(pointLights[i], materialBRDF, normal, viewDirection, position);
    }

    float3 color = Lo / (Lo + float3(1.0f, 1.0f, 1.0f));

    outputColor[pixel] = float4(color, albedo.a);
}
