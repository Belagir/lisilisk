
void vertex()
{
    vec3 rotated = rotate_with_quaterion(VertexPos, InstanceRotation);
    vec3 pos = InstancePosition + (InstanceScale * rotated);

    Normal = rotate_with_quaterion(VertexNormal, InstanceRotation);
    FragPos = pos;
    FragUV = vec2(VertexUV.x, 1. - VertexUV.y);

    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * vec4(pos, 1.0);
}
