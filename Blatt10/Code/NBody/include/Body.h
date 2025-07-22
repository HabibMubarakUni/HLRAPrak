#pragma once

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
    Body(const float px, const float py, const float vx, const float vy, const float m);
};