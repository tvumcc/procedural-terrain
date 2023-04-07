#pragma once
#include <glm/glm.hpp>

enum class Skybox {
    Off = 0,
    Day,
    Night,
};

class Settings {
public:
    Settings();
    void reset();

    // Terrain
    unsigned int seed;
    unsigned int octaves;
    float lacunarity;
    float gain;
    float amplitude;
    float frequency;

    // Lighting
    glm::vec3 terrain_color;
    glm::vec3 light_position;
    glm::vec3 light_color;
    Skybox skybox;

    // Miscellaneous
    float camera_speed;
    bool wireframe_rendering;
    bool capybara_mode;
    bool camera_mode;
};