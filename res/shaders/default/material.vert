
void vertex()
{
    vec3 scaled = VertexPos * InstanceScale;
    vec3 rotated = rotate_with_quaterion(scaled, InstanceRotation);
    vec3 positioned = InstancePosition + rotated;

    Normal = rotate_with_quaterion(VertexNormal, InstanceRotation);
    FragPos = positioned;
    FragUV = vec2(VertexUV.x, 1. - VertexUV.y);

    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * vec4(positioned, 1.0);
}
