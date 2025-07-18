#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;
layout (location = 1) in vec2 VertexUV;
layout (location = 3) in mat4 InstanceMatrix;
// + location 4 for prev. mat4
// + location 5 for prev. mat4
// + location 6 for prev. mat4

uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;

uniform uint TIME;

out vec3 Normal;
out vec3 FragPos;
out vec2 FragUV;

// Rest of the shader code is concatenated after this ------
// ---------------------------------------------------------
// ---------------------------------------------------------
// ---------------------------------------------------------
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
