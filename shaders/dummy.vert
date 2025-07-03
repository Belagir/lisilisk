#version 150
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec4 in_Position;
in vec3 in_Color;

// We output the ex_Color variable to the next shader in the chain
out vec3 ex_Color;

uniform mat4 MODEL_MATRIX;
uniform mat4 PROJECTION_MATRIX;

void main(void) {
    // mat4 mpv_matrix = MODEL_MATRIX * PROJECTION_MATRIX;
    // gl_Position = mpv_matrix * in_Position;
    gl_Position = in_Position;
    ex_Color = in_Color;
}
