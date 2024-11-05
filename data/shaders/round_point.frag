#version 330 core

out vec4 FragColor;

uniform vec4 color;

void main()
{
    float radius = length(gl_PointCoord - vec2(0.5));
    if (radius > 0.5)
        discard;

    FragColor = color;
}