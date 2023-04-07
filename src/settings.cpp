#include "settings.hpp"

Settings::Settings() {
    this->reset();
}

void Settings::reset() {
    this->seed = 31415926535;
    this->octaves = 3;
    this->lacunarity = 2.0f;
    this->gain = 0.5f;
    this->amplitude = 1.0f;
    this->frequency = 1.0f;

    this->terrain_color = glm::vec3(36.0f, 140.0f, 64.0f);
    this->light_position = glm::vec3(3.0f, 10.0f, -4.0f);
    this->light_color = glm::vec3(1.0f, 1.0f, 1.0f);
    this->skybox = Skybox::Night;

    this->camera_speed = 5.0f;
    this->wireframe_rendering = false;
    this->capybara_mode = false;
}