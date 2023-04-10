#include "point_light.hpp"

PointLight::PointLight() {
    reset_settings();
}

void PointLight::reset_settings() {
    color[0] = 1.0f;
    color[1] = 1.0f;
    color[2] = 1.0f;

    position[0] = 3.0f;
    position[1] = 10.0f;
    position[2] = -4.0f;
}