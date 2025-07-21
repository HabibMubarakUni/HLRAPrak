#include "../include/BodiesSOA.h"

BodiesSOA::BodiesSOA(size_t n) {
    posX.reserve(n);
    posY.reserve(n);
    velX.reserve(n);
    velY.reserve(n);
    accX.reserve(n);
    accY.reserve(n);
    mass.reserve(n);
}

void BodiesSOA::add_body(float px, float py, float vx, float vy, float m) {
    posX.push_back(px);
    posY.push_back(py);
    velX.push_back(vx);
    velY.push_back(vy);
    accX.push_back(0.f);
    accY.push_back(0.f);
    mass.push_back(m);
}