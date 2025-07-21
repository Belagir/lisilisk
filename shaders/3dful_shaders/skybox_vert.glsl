#version 330 core

layout (location = 0) in vec3 VertexPos;

out vec3 FragUV;

uniform mat4 PROJECTION_MATRIX;
uniform mat4 VIEW_MATRIX;

void main()
{
    FragUV = VertexPos;
    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * vec4(VertexPos, 1.0);
}
