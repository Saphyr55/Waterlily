#version 450

#extension GL_EXT_nonuniform_qualifier : enable

struct RenderInstance
{
    mat4 model;
    vec4 diffuseFactor;
};

layout(std140, set = 0, binding = 0) uniform View
{
    mat4 view;
    mat4 proj;
    mat4 viewProj;
    vec3 eyePosition;
}
u_View;

layout(std430, set = 1, binding = 0) readonly buffer RenderPacket
{
    RenderInstance items[];
}
b_Packet;

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout(location = 2) in vec2 in_UV;
layout(location = 3) in vec4 in_Tangent;

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec3 out_Normal;
layout(location = 2) out vec2 out_UV;
layout(location = 3) out vec3 out_Tangent;
layout(location = 4) out vec3 out_Bitangent;
layout(location = 5) out flat uint out_InstanceIndex;

void main()
{
    out_InstanceIndex = uint(gl_InstanceIndex);

    mat4 model = b_Packet.items[out_InstanceIndex].model;
    vec4 worldPosition = model * vec4(in_Position, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(model)));

    vec3 normal = normalize(normalMatrix * in_Normal);
    vec3 tangent = normalize(normalMatrix * in_Tangent.xyz);
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    vec3 bitangent = cross(normal, tangent) * in_Tangent.w;

    out_Position = worldPosition.xyz;
    out_Normal = normal;
    out_UV = in_UV;
    out_Tangent = tangent;
    out_Bitangent = bitangent;

    gl_Position = u_View.viewProj * worldPosition;
}
