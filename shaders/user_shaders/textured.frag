
uniform sampler2D base_texture;

void fragment()
{
    FragColor = mix(FragColor, texture(base_texture, FragUV), 0.5);
}
