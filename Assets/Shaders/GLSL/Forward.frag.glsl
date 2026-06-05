#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#define INVALID_TEXTURE 0xffffffff

struct RenderInstance
{
    mat4 model;
    uint materialIndex;
};

struct Material
{
    vec4 diffuseFactor;
    uint diffuse;
    uint normal;
};

struct PunctualLight
{
    vec3 position;
    vec3 color;
};

layout(std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 eyePosition;
}
u_View;

layout(std140, set = 0, binding = 1) uniform Light
{
    PunctualLight punctualLight;
}
u_Light;

layout(std430, set = 1, binding = 0) readonly buffer RenderPacket
{
    RenderInstance items[];
}
b_Packet;

layout(set = 2, binding = 0) uniform sampler2D u_Textures[];

layout(std430, set = 3, binding = 0) readonly buffer MaterialRegistry
{
    Material materials[];
};

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec3 in_Tangent;
layout(location = 4) in vec3 in_Bitangent;
layout(location = 5) in flat uint in_InstanceIndex;

layout(location = 0) out vec4 out_FragColor;

float InverseSquareCurveWindowing(float lightDistance, float maxDistance)
{
    float value = 1.0f - pow(lightDistance / maxDistance, 4);
    float clampedValue = clamp(value, 0.0f, value);
    return clampedValue * clampedValue;
}

float InverseSquareLightAttenuation(float lightDistance, float fixedDistance, float epsilon)
{
    return (fixedDistance * fixedDistance) / ((lightDistance * lightDistance) + epsilon);
}

void main()
{
    RenderInstance item = b_Packet.items[in_InstanceIndex];
    Material material = materials[item.materialIndex];

    vec4 diffuse = texture(u_Textures[nonuniformEXT(material.diffuse)], in_UV);
    diffuse *= material.diffuseFactor;

    vec3 normal = normalize(in_Normal);

    if (material.normal != INVALID_TEXTURE)
    {
        vec3 tangent = normalize(in_Tangent);
        vec3 bitangent = normalize(in_Bitangent);
        mat3 TBN = mat3(tangent, bitangent, normal);

        normal = texture(u_Textures[nonuniformEXT(material.normal)], in_UV).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(TBN * normal);
    }

    vec3 viewDirection = normalize(u_View.eyePosition - in_Position);
    if (dot(viewDirection, normal) < 0.0f)
    {
        normal = -normal;
    }

    PunctualLight punctualLight = u_Light.punctualLight;

    vec3 incident_light = punctualLight.position - in_Position;
    float lightDistance = length(incident_light);
    incident_light = normalize(incident_light);

    float lambertian = max(dot(incident_light, normal), 0.0f);
    vec3 result = diffuse.rgb * lambertian + diffuse.rgb * 0.1;

    float epsilon = 1.0e-5f;
    result *= InverseSquareLightAttenuation(lightDistance, 1.0f, epsilon);

    out_FragColor = vec4(result, diffuse.a);
}
