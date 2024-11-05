#version 330 core

flat in int vertexId;
out vec4 FragColor;

void main()
{
    float radius = length(gl_PointCoord - vec2(0.5));
    if (radius > 0.5)
        discard;

    FragColor = vec4(float(vertexId & 255), float((vertexId >> 8) & 255), float((vertexId >> 16) & 255), 255.0) / 255.0;
}