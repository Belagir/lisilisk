
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ---------------------------------------------------------
// ---------------------------------------------------------
// ---------------------------------------------------------
// rest of the shader code is concatened before this -------

void main()
{
    vertex();

    gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * InstanceMatrix * vec4(VertexPos, 1.0);
}
