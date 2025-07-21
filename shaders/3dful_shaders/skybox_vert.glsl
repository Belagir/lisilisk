#version 330 core

layout (location = 0) in vec3 VertexPos;

out vec3 FragUV;

uniform mat4 PROJECTION_MATRIX;
uniform mat4 VIEW_MATRIX;

void main()
{
    FragUV = VertexPos;

    vec4 normalized_dev_coords = (PROJECTION_MATRIX * mat4(mat3(VIEW_MATRIX)) * vec4(VertexPos, 1.0)).xyww;
    gl_Position = normalized_dev_coords;
}
