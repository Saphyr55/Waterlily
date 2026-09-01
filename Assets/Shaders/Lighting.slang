#include "Shared/Defines.slang"
#include "Shared/DefaultRender.slang"
#include "Shared/Lighting.slang"

WLSL_BINDING(0, 0)
ConstantBuffer<ViewData> View;

WLSL_BINDING(1, 0)
ConstantBuffer<PointLightData[WLSL_MAX_LIGHTS]> PointLights;

WLSL_BINDING(2, 0)
ConstantBuffer<DirectionalLightData> DirectionalLight;

WLSL_BINDING(3, 0)
ConstantBuffer<uint> PointLightCount;

WLSL_BINDING(0, 1)
StructuredBuffer<RenderInstanceData> RenderInstance;

WLSL_BINDING(0, 2)
Texture2D Texture2DRegistry[];
SamplerState TextureSampler;

WLSL_BINDING(0, 3)
StructuredBuffer<MaterialData> Materials;

WLSL_BINDING(0, 4)
Texture2D PositionTexture;

WLSL_BINDING(1, 4)
Texture2D NormalTexture;

WLSL_BINDING(2, 4)
Texture2D AlbedoTexture;

struct FSOutput
{
    float4 Color : SV_Target0;
};

struct VSOutput
{
    float4 Position : SV_Position;
    float2 UV : TEXCOORD0;
};

[shader("vertex")]
VSOutput VSMain(uint vertexID : SV_VertexID)
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

    float3 position = PositionTexture.Sample(TextureSampler, input.UV).rgb;
    float3 normal = NormalTexture.Sample(TextureSampler, input.UV).rgb;
    float3 albedo = AlbedoTexture.Sample(TextureSampler, input.UV).rgb;

    float3 result = float3(0.0f, 0.0f, 0.0f);

    float3 viewDirection = normalize(View.Position - position);

    float fixedDistance = 1.0f;
    float epsilon = 1.0e-5f;

    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    ComputedDirectionalLightData computedDirectionalLight = ComputeDirectionalLight(DirectionalLight, normal);
    result += computedDirectionalLight.Result * albedo;

    for (uint i = 0; i < PointLightCount; i++)
    {
        ComputedPointLightData computedLight = ComputePointLight(PointLights[i], normal, position);
        float attenation = InverseSquareLightAttenuation(computedLight.Distance, fixedDistance, epsilon);
        result += computedLight.Result * albedo * attenation;
    }

    output.Color = float4(result, 1.0f);
    
    return output;
}
