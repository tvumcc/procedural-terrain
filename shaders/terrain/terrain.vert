#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 vp;
uniform float x_pos;
uniform float z_pos;
uniform float last_x_pos;
uniform float last_z_pos;
uniform int permutation_table[512];

vec2 constantVector(int value) {
    if (value == 0) return vec2(1.0, 1.0);
    if (value == 1) return vec2(-1.0, 1.0);
    if (value == 2) return vec2(-1.0, -1.0);
    else return vec2(1.0, -1.0);
}

float lerp(float t, float v1, float v2) {
    return v1 + t*(v2 - v1);
}

// smooths out values (quintic)
float fade(float x) {
    return (6 * pow(x, 5)) - (15 * pow(x, 4)) + (10 * pow(x, 3));
}

float perlinNoise(float x, float y) {
    int xi = int(x);
    int yi = int(y);

    int xr = xi & 255; // Mod 256
    int yr = yi & 255;
    float xf = x - floor(x);
    float yf = y - floor(y);

    // vecs
    vec2 tr = vec2(xf - 1.0, yf - 1.0);
    vec2 tl = vec2(xf, yf - 1.0);
    vec2 br = vec2(xf - 1.0, yf);
    vec2 bl = vec2(xf, yf);

    // hashed values
    int htr = permutation_table[(permutation_table[(xr+1)]+yr+1)];
    int htl = permutation_table[(permutation_table[xr]+yr+1)];
    int hbr = permutation_table[(permutation_table[(xr+1)]+yr)];
    int hbl = permutation_table[(permutation_table[xr]+yr)];

    // dot products
    float dtr = dot(tr, constantVector(htr & 3));
    float dtl = dot(tl, constantVector(htl & 3));
    float dbr = dot(br, constantVector(hbr & 3));
    float dbl = dot(bl, constantVector(hbl & 3));

    float u = fade(xf);
    float v = fade(yf);

    return lerp(u, lerp(v, dbl, dtl), lerp(v, dbr, dtr));
}

out vertex_shader_out {
    vec3 fragment_position;
} vs_out;

void main() {
    float amp = 1.5;
    float freq = 1.0;
    float gain = 0.5;
    float lacunarity = 2.0;

    float total = 0.0;

    vec3 terrain = vec3(aPos.x, 0.0, aPos.z);
    for (int i = 0; i < 6; i++) {
        terrain.y += amp * pow((perlinNoise((aPos.x + x_pos) * freq, (aPos.z + z_pos) * freq) + perlinNoise((aPos.x + last_x_pos) * freq, (aPos.z + last_z_pos) * freq))/2.0, 3);
        total += amp;
        freq *= lacunarity;
        amp *= gain;
    }
    terrain.y /= total;

    gl_Position = vp * model * vec4(terrain, 1.0);
    vs_out.fragment_position = vec3(model * vec4(terrain, 1.0));
}