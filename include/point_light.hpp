#pragma once

struct PointLight {
    PointLight();
    void reset_settings();
    
    float color[3];
    float position[3];
};