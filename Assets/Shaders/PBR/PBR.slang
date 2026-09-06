import Common.Math;
import Common.Light;
import PBR.Material;

using Light;

namespace PBR
{

    public float DistributionGGX(float NdotH, float roughness)
    {
        float alpha = roughness * roughness;
        float alphaSquared = alpha * alpha;
        float NdotHSquared = NdotH * NdotH;

        float numerator = alphaSquared;
        float denominator = NdotH * (alphaSquared - 1.0f) + 1.0f;

        denominator = Math.PI * denominator * denominator;

        return numerator / denominator;
    }

    public float GeometrySmith(float NdotV, float NdotL, float roughness)
    {
        float r = roughness + 1.0;
        float k = (r * r) / 8.0;

        float ggx1 = NdotV / (NdotV * (1.0 - k) + k);
        float ggx2 = NdotL / (NdotL * (1.0 - k) + k);

        return ggx1 * ggx2;
    }

    public float3 FresnelSchlick(float cosTheta, float3 F0)
    {
        return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
    }

    public float3 CookTorranceBRDF(ILight light,
                                   MaterialBRDF material,
                                   float3 normal, 
                                   float3 viewDirection, 
                                   float3 worldPosition)
    {
        float3 lightDirection = light.getDirection(worldPosition);
        float3 radiance = light.getRadiance(worldPosition);
        float3 halfway = normalize(viewDirection + lightDirection);

        float NdotL = max(dot(normal, lightDirection), 0.0);
        float NdotV = max(dot(normal, viewDirection), 0.0);
        float NdotH = max(dot(normal, halfway), 0.0);
        float HdotV = max(dot(halfway, viewDirection), 0.0);

        float D = PBR.DistributionGGX(NdotH, material.roughness);
        float G = PBR.GeometrySmith(NdotV, NdotL, material.roughness);
        float3 F = PBR.FresnelSchlick(HdotV, material.F0);

        float3 numerator = D * G * F;
        float denominator = 4.0 * NdotV * NdotL + Math.Epsilon;
        float3 specular = numerator / denominator;

        float3 kS = F;
        float3 kD = float3(1.0, 1.0, 1.0) - kS;
        kD *= 1.0 - material.metallic;

        return (kD * material.albedo / Math.PI + specular) * radiance * NdotL;
    }

}
