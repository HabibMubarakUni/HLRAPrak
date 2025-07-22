void kernel compute_forces(
    global const float* posX, 
    global const float* posY, 
    global float* accX, 
    global float* accY, 
    global const float* mass,
    const int n_bodies,
    const float G, 
    const float eps)
{
    const int i = get_global_id(0);
    if (i >= n_bodies) {
        return;
    }

    accX[i] = 0.f;
    accY[i] = 0.f;

    for (int j = 0; j < n_bodies; ++j) {
        if (i != j) {
            const float dx = posX[j] - posX[i];
            const float dy = posY[j] - posY[i];

            const float distSqr = dx * dx + dy * dy + eps * eps;
            const float dist = sqrt(distSqr);
            const float invDistCubed = 1.f / (dist * dist * dist);

            const float f = G * mass[j] * invDistCubed;

            accX[i] += dx * f;
            accY[i] += dy * f;
        }
    }
}

void kernel integrate_bodies(
    global float* posX,
    global float* posY,
    global float* velX,
    global float* velY,
    global const float* accX,
    global const float* accY,
    const int n_bodies,
    const float dt,
    const int WIDTH,
    const int HEIGHT)
{
    const int i = get_global_id(0);
    if (i >= n_bodies) {
        return;
    }

    velX[i] += accX[i] * dt;
    velY[i] += accY[i] * dt;

    posX[i] += velX[i] * dt;
    posY[i] += velY[i] * dt;

    const float floatWidth = (float)WIDTH;
    const float floatHeight = (float)HEIGHT;

    if (posX[i] > (floatWidth / 2.f)) {
        posX[i] -= floatWidth;
    }
    else if (posX[i] < -(floatWidth / 2.f)) {
        posX[i] += floatWidth;
    }

    if (posY[i] > (floatHeight / 2.f)) {
        posY[i] -= floatHeight;
    }
    else if (posY[i] < -(floatHeight / 2.f)) {
        posY[i] += floatHeight;
    }
}
