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
        posX.reserve(n);
        posY.reserve(n);
        velX.reserve(n);
        velY.reserve(n);
        accX.reserve(n);
        accY.reserve(n);
        mass.reserve(n);
    }
};
