
uniform sampler2D height_texture;

mat4 scale_matrix(mat4 matrix, vec3 scale)
{
    matrix[0].x *= scale.x;
    matrix[1].y *= scale.y;
    matrix[2].z *= scale.z;

    return matrix;
}

void vertex()
{
    vec2 tmp_pos = (vec2(InstanceMatrix[3].x, InstanceMatrix[3].z) + vec2(50, 50)) / 100.;
    float zscale = .1 + (1-texture(height_texture, tmp_pos).r) * .9;
    mat4 scaled_matrix = scale_matrix(InstanceMatrix, vec3(1, zscale, 1));

    Normal = normalize(mat3(transpose(inverse(scaled_matrix))) * VertexNormal);
    FragPos = vec3(scaled_matrix * vec4(VertexPos, 1.0));
    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * scaled_matrix * vec4(VertexPos, 1.0);
}
