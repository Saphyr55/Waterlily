static const int kMaxPointLight = 32;

struct PointLight
{
    float3 Position;
    float3 Color;
};

struct ComputedPointLight
{
    float3 Result;
    float3 Direction;
    float Intensity;
    float Distance;
};

struct DirectionalLight
{
    float3 Direction;
    float3 Color;
};

struct ComputedDirectionalLight
{
    float3 Result;
    float Intensity;
};

ComputedDirectionalLight ComputeDirectionalLight(DirectionalLight light, float3 normal)
{
    ComputedDirectionalLight outputLight;

    float3 lightDirection = normalize(light.Direction);
    float lambertian = dot(lightDirection, normal);

    outputLight.Intensity = saturate(lambertian);
    outputLight.Result = outputLight.Intensity * light.Color;

    return outputLight;
}

ComputedPointLight ComputePointLight(PointLight light, float3 normal, float3 position) 
{
    ComputedPointLight outputLight;

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
