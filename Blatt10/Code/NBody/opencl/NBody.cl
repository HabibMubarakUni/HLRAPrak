void kernel compute_forces(
    global const std::vector<float>& posX, 
    global const std::vector<float>& posY, 
    global std::vector<float>& accX, 
    global std::vector<float>& accY, 
    global const std::vector<float>& mass,
    const size_t n_bodies,
    const float G, 
    const float eps){
        const int i{get_global_id(0)};

        bodies[i].accX = 0.f;
        bodies[i].accY = 0.f;
    
        float dx, dy, distSqr, dist, invDistCubed, f;

        for (size_t j = 0; j < n_bodies; ++j) {

            if (i != j) {
                Body& bi = bodies[i];
                const Body& bj = bodies[j];

                // 1) Calculate distances in x and y to the other body
                dx = bj.posX - bi.posX;
                dy = bj.posY - bi.posY;
                
                // 2) Calculate the cubed inverse distance, including the softening factor eps
                distSqr = dx * dx + dy * dy + eps * eps;
                dist = std::sqrt(distSqr);
                invDistCubed = 1.f / (dist * dist * dist);
                
                // 3) Calculate the force applied from body Bj to body Bi
                f = G * bj.mass * invDistCubed;

                // 4) Sum and update the acceleration in both directions for body Bi
                bi.accX += dx * f;
                bi.accY += dy * f;
            }
        }
}


void integrate_bodies(bodies, float dt){

}