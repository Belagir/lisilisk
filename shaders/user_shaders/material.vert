
void vertex()
{
    Normal = normalize(mat3(transpose(inverse(InstanceMatrix))) * VertexNormal);
    FragPos = vec3(InstanceMatrix * vec4(VertexPos, 1.0));
}
