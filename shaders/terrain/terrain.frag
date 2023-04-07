#version 330 core
out vec4 FragColor;

in vec3 norm;
in vec3 fragment_position;

uniform vec3 color;
uniform vec3 light_pos;
uniform vec3 light_color;

void main() {
    vec3 light_direction = normalize(light_pos - fragment_position);
    
    vec3 diffuse_lighting = vec3(max(dot(norm, light_direction), 0.0));

    vec3 final = (vec3(0.3, 0.3, 0.3) + (diffuse_lighting * light_color)) * color;
    FragColor = vec4(final, 1.0);
}