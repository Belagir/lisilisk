#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;

uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;

out vec3 Normal;
out vec3 FragPos;

void main()
{
    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * MODEL_MATRIX * vec4(VertexPos, 1.0);

    FragPos = vec3(MODEL_MATRIX * vec4(VertexPos, 1.0));
    Normal = normalize(mat3(transpose(inverse(MODEL_MATRIX))) * VertexNormal);
}
