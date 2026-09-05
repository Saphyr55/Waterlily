#define WLSL_MAX_LIGHTS 32

struct PointLightData
{
    float3 Position;
    float3 Color;
};

struct ComputedPointLightData
{
    float3 Result;
    float3 Direction;
    float Intensity;
    float Distance;
};

struct DirectionalLightData
{
    float3 Direction;
    float3 Color;
};

struct ComputedDirectionalLightData
{
    float3 Result;
    float Intensity;
};

ComputedDirectionalLightData ComputeDirectionalLight(DirectionalLightData light, float3 normal)
{
    ComputedDirectionalLightData outputLight;

    float3 lightDirection = normalize(light.Direction);
    float lambertian = dot(lightDirection, normal);

    outputLight.Intensity = saturate(lambertian);
    outputLight.Result = outputLight.Intensity * light.Color;

    return outputLight;
}

ComputedPointLightData ComputePointLight(PointLightData light, float3 normal, float3 position) 
{
    ComputedPointLightData outputLight;

    float3 lightDirection = light.Position - position;
    outputLight.Distance = length(lightDirection);
    outputLight.Direction = normalize(lightDirection);

    float lambertian = dot(outputLight.Direction, normal);

    outputLight.Intensity = saturate(lambertian);
    outputLight.Result = outputLight.Intensity * light.Color;   

    return outputLight;
}

float InverseSquareLightAttenuation(float lightDistance, float fixedDistance, float epsilon)
{
    return (fixedDistance * fixedDistance) / ((lightDistance * lightDistance) + epsilon);
}
