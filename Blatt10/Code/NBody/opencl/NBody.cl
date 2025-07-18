__kernel void compute_forces(
    __global const float* posX, 
    __global const float* posY, 
    __global float* accX, 
    __global float* accY, 
    __global const float* mass,
    const uint n_bodies,
    const float G, 
    const float eps)
{
    int i = get_global_id(0);

    accX[i] = 0.f;
    accY[i] = 0.f;

    float dx, dy, distSqr, dist, invDistCubed, f;

    for (uint j = 0; j < n_bodies; ++j) {
        if (i != j) {

            dx = posX[j] - posX[i];
            dy = posY[j] - posY[i];

            distSqr = dx * dx + dy * dy + eps * eps;
            dist = sqrt(distSqr);

            invDistCubed = 1.f / (dist * dist * dist);

            f = G * mass[j] * invDistCubed;

            accX[i] += dx * f;
            accY[i] += dy * f;
        }
    }
}



__kernel void integrate_bodies(
    __global float* posX,
    __global float* posY,
    __global float* velX,
    __global float* velY,
    __global const float* accX,
    __global const float* accY,
    const float dt)
{
    int i = get_global_id(0);

    velX[i] += accX[i] * dt;
    velY[i] += accY[i] * dt;

    posX[i] += velX[i] * dt;
    posY[i] += velY[i] * dt;
}
