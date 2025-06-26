#version 150
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
in vec4 in_Position;
in vec3 in_Color;

// We output the ex_Color variable to the next shader in the chain
out vec3 ex_Color;


void main(void) {
    gl_Position = in_Position;
    ex_Color = in_Color;
}
