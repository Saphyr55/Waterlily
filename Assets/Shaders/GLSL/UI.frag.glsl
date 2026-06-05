#version 450

layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec4 in_Color;

layout(location = 0) out vec4 out_FragColor;

void main()
{
    out_FragColor = in_Color;
}