#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform vec3 color;
uniform vec3 light_color;
uniform sampler2D water;

void main() {
    FragColor = vec4(vec3(17.0 / 255.0, 64.0 / 255.0, 96.0 / 255.0) + (color * light_color), 1.0);
}