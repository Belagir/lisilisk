#version 330 core

layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in mat4 InstanceMatrix;
// + location 3 for prev. mat4
// + location 4 for prev. mat4
// + location 5 for prev. mat4

uniform mat4 VIEW_MATRIX;
uniform mat4 PROJECTION_MATRIX;

uniform uint TIME;

out vec3 Normal;
out vec3 FragPos;

mat4 scale_matrix(mat4 matrix, vec3 scale)
{
    matrix[0].x *= scale.x;
    matrix[1].y *= scale.y;
    matrix[2].z *= scale.z;

    return matrix;
}

void main()
{
    float zscale = .8+.4*sin((float(TIME)/100.));
    mat4 scaled_matrix = scale_matrix(InstanceMatrix, vec3(1, zscale, 1));

    Normal = normalize(mat3(transpose(inverse(scaled_matrix))) * VertexNormal);
    FragPos = vec3(scaled_matrix * vec4(VertexPos, 1.0));

    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * scaled_matrix * vec4(VertexPos, 1.0);
}
