#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in vertex_shader_out {
    vec3 fragment_position;
} gs_in[];

out vec3 fragment_position;
out vec3 color;
out vec3 norm;

vec3 compute_color(float height) {
    if (height >= 0.3) {
        return vec3(200.0, 200.0, 200.0) / 255.0;
    } else if (height >= 0.02) {
        return vec3(100.0, 100.0, 100.0) / 255.0;
    } else if (height >= -0.01) {
        return vec3(36.0, 140.0, 64.0) / 255.0;
    } else {
        return vec3(255.0, 224.0, 138.0) / 255.0;
    }
}

void main() {
    //if (!(gs_in[0].fragment_position.y < -0.15 && gs_in[1].fragment_position.y < -0.15 && gs_in[2].fragment_position.y < -0.15)) {
        // flat shading
        norm = normalize(cross(gs_in[1].fragment_position - gs_in[0].fragment_position, gs_in[2].fragment_position - gs_in[0].fragment_position));

        gl_Position = gl_in[0].gl_Position;
        fragment_position = gs_in[0].fragment_position;
        color = compute_color(gs_in[0].fragment_position.y);
        EmitVertex();

        gl_Position = gl_in[1].gl_Position;
        fragment_position = gs_in[1].fragment_position;
        color = compute_color(gs_in[1].fragment_position.y);
        EmitVertex();

        gl_Position = gl_in[2].gl_Position;
        fragment_position = gs_in[2].fragment_position;
        color = compute_color(gs_in[2].fragment_position.y);
        EmitVertex();
        EndPrimitive();
    //}
}