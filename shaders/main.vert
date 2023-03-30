#version 330 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 vp;
uniform int permutation_table[256];

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
    int htr = permutation_table[(permutation_table[(xr+1) & 255]+yr+1) & 255];
    int htl = permutation_table[(permutation_table[xr & 255]+yr+1) & 255];
    int hbr = permutation_table[(permutation_table[(xr+1) & 255]+yr) & 255];
    int hbl = permutation_table[(permutation_table[xr & 255]+yr) & 255];

    // dot products
    float dtr = dot(tr, constantVector(htr & 3));
    float dtl = dot(tl, constantVector(htl & 3));
    float dbr = dot(br, constantVector(hbr & 3));
    float dbl = dot(bl, constantVector(hbl & 3));

    float u = fade(xf);
    float v = fade(yf);

    return lerp(u, lerp(v, dbl, dtl), lerp(v, dbr, dtr));
}

void main() {
    gl_Position = vp * model * vec4(vec3(aPos.x, perlinNoise(aPos.x * 0.5, aPos.z * 0.5), aPos.z), 1.0);
}