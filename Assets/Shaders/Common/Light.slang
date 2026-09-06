import Common.Math;

namespace Light
{

    public static const int kMaxPointLight = 32;

    public float InverseSquareLightAttenuation(float lightDistance, float fixedDistance, float epsilon)
    {
        return (fixedDistance * fixedDistance) / ((lightDistance * lightDistance) + epsilon);
    }

    public interface ILight
    {
        public float3 getRadiance(float3 worldPosition);

        public float3 getDirection(float3 worldPosition);
    }

    public struct PointLight : ILight
    {
        public float3 position;
        public float3 color;

        public float3 getRadiance(float3 worldPosition)
        {
            float3 lightDirection = this.position - worldPosition;
            float distance = length(lightDirection);
            lightDirection = normalize(lightDirection);
            float attenuation = 1.0 / ((distance * distance) + Math.Epsilon);

            return attenuation * this.color;
        }

        public float3 getDirection(float3 worldPosition)
        {
            return normalize(this.position - worldPosition);
        }
    }

    public struct DirectionalLight : ILight
    {
        public float3 direction;
        public float3 color;

        public float3 getRadiance(float3 worldPosition)
        {
            return color;
        }

        public float3 getDirection(float3 worldPosition)
        {
            return normalize(this.direction);
        }
    }

}
