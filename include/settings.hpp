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
    int seed;
    unsigned int octaves;
    float lacunarity;
    float gain;
    float amplitude;
    float frequency;
    float fudge;
    float water_level;

    // Lighting
    float terrain_color[3];
    float light_position[3];
    float light_color[3];
    Skybox skybox;

    // Miscellaneous
    float camera_speed;
    bool wireframe_rendering;
    bool capybara_mode;
    bool camera_mode;
};