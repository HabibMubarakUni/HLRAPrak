#include "../include/Body.h"
#include <cmath>

Body::Body(float px, float py, float vx, float vy, float m)
    : posX(px), posY(py), velX(vx), velY(vy), accX(0.f), accY(0.f), mass(m) {}
