
void fragment()
{
    FragColor = TextureContribution * LightContribution;
    FragColor = FragColor + EmissionContribution;
    FragColor = mix(FragColor, FogContribution, FogContribution.a);
}
