#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 vp;
uniform float time;

out vertex_shader_out {
    vec3 fragment_position;
} vs_out;

void main() {
    float amplitude = 0.005;
    float frequency = 1.0;
    float t = time * 2.0;
    float height = sin(aPos.x*frequency); 
    height += sin(aPos.x*frequency*2.1 + t    ) * 4.5;
    height += sin(aPos.x*frequency*1.7 + t*1.1) * 4.0;
    height += sin(aPos.x*frequency*2.2 + t*0.4) * 5.0;
    height += sin(aPos.x*frequency*3.1 + t*4.3) * 2.5;
    height += sin(aPos.z*frequency*2.1 + t*1.0) * 3.124;
    height += sin(aPos.z*frequency*2.3 + t*1.1) * 1.16;
    height += sin(aPos.z*frequency*2.2 + t*0.4) * 7.3;
    height += sin(aPos.z*frequency*3.1 + t*4.3) * 2.425;
    height *= amplitude;    

    vec3 water = vec3(aPos.x, 0.055, aPos.z);
    gl_Position = vp * model * vec4(water, 1.0);
    vs_out.fragment_position = vec3(model * vec4(water, 1.0));
}