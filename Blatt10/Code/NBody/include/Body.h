#pragma once
#include <cstddef>
#include <cstdlib>
#include <immintrin.h> // für _mm_malloc/_mm_free

struct Body {
    // Position
    float posX;
    float posY;

    // Velocity
    float velX;
    float velY;

    // Acceleration
    float accX;
    float accY;

    // Mass
    float mass;

    // Konstruktor
    Body(float px, float py, float vx, float vy, float m);
};