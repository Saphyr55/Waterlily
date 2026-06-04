#version 450

struct DrawItemData {
    mat4 model;
    vec4 diffuse_factor;
};

layout(std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 proj;
    mat4 view_proj;
    vec3 eye_position;
}
u_view;

layout(std430, set = 1, binding = 0) readonly buffer DrawPacketData
{
    DrawItemData items[];
}
b_packet;

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec2 in_texture_uv;
layout(location = 3) in vec4 in_tangent;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec3 out_normal;
layout(location = 2) out vec2 out_texture_uv;
layout(location = 3) out vec3 out_tangent;
layout(location = 4) out vec3 out_bitangent;
layout(location = 5) out flat uint out_draw_item_id;

void main()
{
    out_draw_item_id = uint(gl_InstanceIndex);

    mat4 model = b_packet.items[out_draw_item_id].model;
    vec4 world_position = model * vec4(in_position, 1.0);

    mat3 normal_matrix = transpose(inverse(mat3(model)));

    vec3 normal = normalize(normal_matrix * in_normal);
    vec3 tangent = normalize(normal_matrix * in_tangent.xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent) * in_tangent.w;

    out_position = world_position.xyz;
    out_bitangent = bitangent;
    out_normal = normal;
    out_tangent = tangent;
    out_texture_uv = in_texture_uv;

    gl_Position = u_view.view_proj * world_position;
}
