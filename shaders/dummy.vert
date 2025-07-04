#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;

out vec3 Normal;

void main()
{
    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * MODEL_MATRIX * vec4(aPos, 1.0);
    Normal = aNormal;
}
