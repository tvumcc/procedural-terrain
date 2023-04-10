#pragma once
#include <vector>
#include "shader.hpp"
#include "camera.hpp"
#include "water.hpp"
#include "point_light.hpp"

class Terrain {
public:
    Terrain(float width, float height, int subdivisions);
    void draw(Shader& shader, Camera& camera, PointLight& light);
    void reset_settings();

    float amplitude;
    float frequency;
    float gain;
    float lacunarity;
    float contrast;
    int seed;
    bool wireframe_rendering;

    float get_width() {return width;}
    float get_height() {return height;}
    int get_subdivisions() {return subdivisions;}
private:
    float width;
    float height;
    int subdivisions;

    unsigned int VAO;
    unsigned int VBO;

    std::vector<float> mesh_vertices;
    void generate_mesh();
};