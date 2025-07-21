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
    Body(float px, float py, float vx, float vy, float m);
};