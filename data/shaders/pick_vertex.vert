#version 330 core

layout(location = 0) in vec3 position;

flat out int vertexId;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vertexId = gl_VertexID + 1;
    gl_Position = projection * view * model * vec4(position, 1.0);
}