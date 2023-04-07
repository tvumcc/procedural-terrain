#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 vp;

void main() {
    vec3 water = vec3(aPos.x, 0.055, aPos.z);
    gl_Position = vp * model * vec4(water, 1.0);
}