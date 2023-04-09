#include "settings.hpp"

Settings::Settings() {
    this->reset();
    this->camera_mode = true;
}

void Settings::reset() {
    // Default settings
    this->seed = 31415926535;
    this->octaves = 6;
    this->lacunarity = 2.0f;
    this->gain = 0.5f;
    this->amplitude = 1.0f;
    this->frequency = 1.0f;
    this->fudge = 1.2f;
    this->water_level = 0.055f;

    this->terrain_color[0] = 36.0f;
    this->terrain_color[1] = 140.0f;
    this->terrain_color[2] = 64.0f;
    this->light_position[0] = 3.0f;
    this->light_position[1] = 10.0f;
    this->light_position[2] = -4.0f;
    this->light_color[0] = 1.0f;
    this->light_color[1] = 1.0f;
    this->light_color[2] = 1.0f;
    this->skybox = Skybox::Night;

    this->camera_speed = 5.0f;
    this->wireframe_rendering = false;
    this->capybara_mode = false;
}