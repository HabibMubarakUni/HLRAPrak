#include "../include/Body.h"

Body::Body(const float px, const float py, const float vx, const float vy, const float m)
    : posX{px}, posY{py}, velX{vx}, velY{vy}, accX{0.f}, accY{0.f}, mass{m} {}
