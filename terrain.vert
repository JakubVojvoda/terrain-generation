#version 130

in vec3 position;
in vec3 color;

uniform mat4 mvp;

out vec3 c;

void main() {

    gl_Position = mvp * vec4(position, 1);
    c = color;
}
