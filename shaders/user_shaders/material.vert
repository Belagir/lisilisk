
void vertex()
{
    Normal = normalize(mat3(transpose(inverse(InstanceMatrix))) * VertexNormal);
    FragPos = vec3(InstanceMatrix * vec4(VertexPos, 1.0));
    FragUV = vec2(VertexUV.x, 1. - VertexUV.y);

    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * InstanceMatrix * vec4(VertexPos, 1.0);
}
