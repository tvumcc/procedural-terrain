#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;


in vertex_shader_out {
    vec3 fragment_position;
} gs_in[];

out vec3 fragment_position;
out vec3 norm;

void main() {
    //if (!(gs_in[0].fragment_position.y < -0.15 && gs_in[1].fragment_position.y < -0.15 && gs_in[2].fragment_position.y < -0.15)) {
        // flat shading
        norm = normalize(cross(gs_in[1].fragment_position - gs_in[0].fragment_position, gs_in[2].fragment_position - gs_in[0].fragment_position));

        gl_Position = gl_in[0].gl_Position;
        fragment_position = gs_in[0].fragment_position;

        EmitVertex();

        gl_Position = gl_in[1].gl_Position;
        fragment_position = gs_in[1].fragment_position;
        EmitVertex();

        gl_Position = gl_in[2].gl_Position;
        fragment_position = gs_in[2].fragment_position;
        EmitVertex();
        EndPrimitive();
    //}
}