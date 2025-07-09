#include "../include/Body.h"
#include <cmath>

void compute_forces(std::vector<Body>& bodies, float G, float eps) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].accX = 0.f;
        bodies[i].accY = 0.f;
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
    for (size_t i = 0; i < bodies.size(); ++i) {
        // Geschwindigkeit aktualisieren
        bodies[i].velX += bodies[i].accX * dt;
        bodies[i].velY += bodies[i].accY * dt;

        // Position aktualisieren
        bodies[i].posX += bodies[i].velX * dt;
        bodies[i].posY += bodies[i].velY * dt;
    }
}