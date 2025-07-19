
// ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
// ---------------------------------------------------------
// ---------------------------------------------------------
// ---------------------------------------------------------
// rest of the shader code is concatened before this -------

void main()
{
    LightContribution = light_ambient_contribution(LIGHT_AMBIENT);

    for (uint i = 0u ; i < LIGHT_POINTS_NB ; i++) {
        LightContribution += light_point_contribution(LIGHT_POINTS.array[i]);
    }
    for (uint i = 0u ; i < LIGHT_DIRECTIONALS_NB ; i++) {
        LightContribution += light_directional_contribution(LIGHT_DIRECTIONALS.array[i]);
    }

    FragColor = LightContribution;

    fragment();
}
