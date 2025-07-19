
vec4 emissive_contribution()
{
    return vec4(MATERIAL.emissive, 1.)
            * texture(emissive_mask, FragUV)
            * MATERIAL.emissive_strength;
}

void fragment()
{
    vec4 emission = emissive_contribution();

    FragColor = mix(FragColor * texture(base_texture, FragUV), emission, length(emission.rgb));
}
