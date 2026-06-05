#version 450

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_Color;

layout(location = 0) out vec3 out_Position;
layout(location = 1) out vec4 out_Color;

void main()
{
    vec4 position = vec4(in_Position, 1.0);

    out_Position = position.xyz;
    out_Color = in_Color;

    gl_Position = position;
}