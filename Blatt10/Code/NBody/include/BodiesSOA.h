#pragma once
#include <vector>
#include <cstddef>

struct BodiesSOA {
    std::vector<float> posX;
    std::vector<float> posY;
    std::vector<float> velX;
    std::vector<float> velY;
    std::vector<float> accX;
    std::vector<float> accY;
    std::vector<float> mass;

    BodiesSOA();

    void reserve(size_t n);
    
    void add_body(float px, float py, float vx, float vy, float m);
};