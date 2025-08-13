#version 330 core

// ---------------------------------------------------------
// ---------------------------------------------------------

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexUV;
layout (location = 3) in vec3 InstancePosition;
layout (location = 4) in vec3 InstanceScale;
layout (location = 5) in vec4 InstanceRotation;

uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;

uniform uint TIME;

out vec3 Normal;
out vec3 FragPos;
out vec2 FragUV;

// ---------------------------------------------------------
// ---------------------------------------------------------

vec3 rotate_with_quaterion(vec3 v, vec4 q)
{
	return v + 2. * cross(cross(v, q.xyz ) + q.w*v, q.xyz);
}

// Rest of the shader code is concatenated after this ------
// ---------------------------------------------------------
// ---------------------------------------------------------
// ---------------------------------------------------------
// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv
