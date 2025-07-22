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

    void reserve(const size_t n);
    
    void add_body(const float px, const float py, const float vx, const float vy, const float m);
};