#pragma once
#include <vector>

class Body {
public:
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
    Body(float x, float y, float vx, float vy, float m);

    // Methoden
    void applyForce(float forceX, float forceY);
    void update(float deltaTime);

};

// compute_forces deklarieren
void compute_forces(std::vector<Body>& bodies, float G, float eps);

// updates of Body Positions
void integrate_bodies(std::vector<Body>& bodies, float dt);