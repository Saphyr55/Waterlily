#version 450

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec4 in_color;

layout(location = 0) out vec3 out_position;
layout(location = 1) out vec4 out_color;

void main()
{
    vec4 position = vec4(in_position, 1.0);

    out_position = position.xyz;
    out_color = in_color;

    gl_Position = position;
}