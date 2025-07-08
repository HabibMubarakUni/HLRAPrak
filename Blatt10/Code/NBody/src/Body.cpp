#include "../include/Body.h"
#include <cmath>

void compute_forces(std::vector<Body>& bodies, float G, float eps) {
    for (auto& bi : bodies) {
        bi.accX = 0.f;
        bi.accY = 0.f;
    }

    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = 0; j < bodies.size(); ++j) {

            if (i != j){
            Body& bi = bodies[i];
            const Body& bj = bodies[j];

            // 1) Calculate distances in x and y to the other body
            float dx = bj.posX - bi.posX;
            float dy = bj.posY - bi.posY;
            
            // 2) Calculate the cubed inverse distance, including the softening factor eps
            float distSqr = dx * dx + dy * dy + eps * eps;
            float dist = std::sqrt(distSqr);
            float invDistCubed = 1.f / (dist * dist * dist);
            
            // 3) Calculate the force applied from body Bj to body Bi
            float f = G * bj.mass * invDistCubed;

            // 4) Sum and update the acceleration in both directions for body Bi
            bi.accX += dx * f;
            bi.accY += dy * f;
            }
        }
    }
}

void integrate_bodies(std::vector<Body>& bodies, float dt) {
    for (auto& b : bodies) {
        // Geschwindigkeit aktualisieren
        b.velX += b.accX * dt;
        b.velY += b.accY * dt;

        // Position aktualisieren
        b.posX += b.velX * dt;
        b.posY += b.velY * dt;
    }
}