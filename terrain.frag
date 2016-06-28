#version 130

in vec3 c;

out vec4 fragColor;

void main()
{
    fragColor = vec4(c,1);
}
