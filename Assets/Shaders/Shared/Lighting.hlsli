
struct PunctualLight
{
    float3 Position;
    float3 Color;
};

struct ComputedPunctualLight
{
    float3 Direction;
    float Distance;
    float Lambertian;
    float3 Result;
};

float InverseSquareLightAttenuation(float lightDistance, float fixedDistance, float epsilon)
{
    return (fixedDistance * fixedDistance) / ((lightDistance * lightDistance) + epsilon);
}

ComputedPunctualLight ComputePunctualLight(PunctualLight light, float3 normal, float3 position, float3 eyeDirection) 
{
    ComputedPunctualLight output;

    if (dot(eyeDirection, normal) < 0.0f)
    {
        normal = -normal;
    }
    
    float3 lightDirection = light.Position - position;
    output.Distance = length(lightDirection);
    output.Direction = normalize(lightDirection);
    
    output.Lambertian = max(dot(output.Direction, normal), 0.0f);
    output.Result = output.Lambertian * light.Color;   

    return output;
}
