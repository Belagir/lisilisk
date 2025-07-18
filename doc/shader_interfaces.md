
# Shader system

User-supplied shaders are simplified files that won't include default uniforms,
input / output, and `main()` definition.

In your shaders, you should define either `void vertex()` or `void fragment()`,
alongside your own variables (uniforms, in/out, etc) to customize the behavior
of your render. The system will provide the default uniforms and variables at
compilation time.

## Vertex Shader defaults

**Vertex attributes:**

- `vec3 VertexPos` : on layout 0.
- `vec3 VertexNormal` : on layout 1.
- `vec2 VertexUV` : on layout 2.
- `mat4 InstanceMatrix` : on layout 3, 4, 5, and 6.

**Uniforms:**

- `mat4 VIEW_MATRIX`
- `mat4 PROJECTION_MATRIX`
- `uint TIME`

**Outputs:**

- `vec3 Normal`
- `vec3 FragPos`
- `vec2 FragUV`

## Fragment Shader default

**Uniforms:**

- `vec3 CAMERA_POS`
- `uint TIME`
- `vec4 MATERIAL.ambient`
- `vec4 MATERIAL.diffuse`
- `vec4 MATERIAL.specular`
- `float MATERIAL.shininess`

**Inputs:**

- `vec3 Normal`
- `vec3 FragPos`
- `vec2 FragUV`
- `vec4 LightContribution`

**Outputs:**

- `vec4 FragColor`
