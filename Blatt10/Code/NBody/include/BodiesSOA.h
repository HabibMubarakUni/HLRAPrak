#pragma once
#include <vector>

struct BodiesSOA {
    std::vector<float> posX;
    std::vector<float> posY;
    std::vector<float> velX;
    std::vector<float> velY;
    std::vector<float> accX;
    std::vector<float> accY;
    std::vector<float> mass;

    BodiesSOA(size_t n) {
        posX.resize(n);
        posY.resize(n);
        velX.resize(n);
        velY.resize(n);
        accX.resize(n);
        accY.resize(n);
        mass.resize(n);
    }
};