#pragma once
#include "shader.hpp"
#include "camera.hpp"
#include "point_light.hpp"

class Water {
public:
    Water();
    void reset_settings();
    void draw(Shader& shader, Camera& camera, PointLight& light, float width, float height);

    float water_level;
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glm::vec3 color;
};