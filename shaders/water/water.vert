#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord; 

out vec2 TexCoord;

uniform mat4 model;
uniform mat4 vp;
uniform float water_level;

void main() {
    vec3 water = vec3(aPos.x, water_level, aPos.z);
    gl_Position = vp * model * vec4(water, 1.0);
    TexCoord = aTexCoord;
}