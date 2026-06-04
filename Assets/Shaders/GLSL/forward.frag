#version 450

#extension GL_EXT_nonuniform_qualifier : enable

#define INVALID_TEXTURE 0xffffffff

struct DrawItemData {
    mat4 model;
    uint material_index;
};

struct MaterialData {
    vec4 diffuse_factor;
    uint diffuse;
    uint normal;
};

struct PunctualLight {
    vec3 position;
    vec3 color;
};

layout(std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
}
u_view;

layout(std140, set = 0, binding = 1) uniform Lights
{
    PunctualLight punctual_light;
}
u_lights;

layout(std430, set = 1, binding = 0) readonly buffer DrawPacketData
{
    DrawItemData items[];
}
b_packet;

layout(set = 2, binding = 0) uniform sampler2D u_texture_registry[];

layout(std430, set = 3, binding = 0) readonly buffer MaterialRegistry
{
    MaterialData materials[];
};

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;
layout(location = 3) in vec3 in_tangent;
layout(location = 4) in vec3 in_bitangent;
layout(location = 5) in flat uint in_draw_item_id;

layout(location = 0) out vec4 out_frag_color;

float inverse_square_curve_windowing(float light_distance, float max_distance)
{
    float value = 1.0f - pow(light_distance / max_distance, 4);
    float clamped_value = clamp(value, 0.0f, value);
    return clamped_value * clamped_value;
}

float inverse_square_light_attenuation(float light_distance, float fixed_distance, float epsilon)
{
    return (fixed_distance * fixed_distance) / ((light_distance * light_distance) + epsilon);
}

void main()
{
    DrawItemData item = b_packet.items[in_draw_item_id];
    MaterialData material = materials[item.material_index];

    vec4 diffuse = texture(u_texture_registry[nonuniformEXT(material.diffuse)], in_texture_uv);
    diffuse *= material.diffuse_factor;

    vec3 normal = normalize(in_normal);

    if (material.normal != INVALID_TEXTURE) {
        vec3 tangent = normalize(in_tangent);
        vec3 bitangent = normalize(in_bitangent);
        mat3 TBN = mat3(tangent, bitangent, normal);

        normal = texture(u_texture_registry[nonuniformEXT(material.normal)], in_texture_uv).rgb;
        normal = normal * 2.0f - 1.0f;
        normal = normalize(TBN * normal);
    }

    vec3 view_direction = normalize(u_view.eye_position - in_position);
    if (dot(view_direction, normal) < 0.0f) {
        normal = -normal;
    }

    PunctualLight punctual_light = u_lights.punctual_light;

    vec3 incident_light = punctual_light.position - in_position;
    float light_distance = length(incident_light);
    incident_light = normalize(incident_light);

    float lambertian = max(dot(incident_light, normal), 0.0f);
    vec3 result = diffuse.rgb * lambertian + diffuse.rgb * 0.1;

    float epsilon = 1.0e-5f;
    result *= inverse_square_light_attenuation(light_distance, 1.0f, epsilon);

    out_frag_color = vec4(result, diffuse.a);
}
