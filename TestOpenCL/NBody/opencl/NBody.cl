void kernel matrix_multiply(
    global const float* A, 
    global const float* B, 
    global float* C, 
    const int N) 
{
    int row = get_global_id(0);
    int col = get_global_id(1);

    float sum = 0.0f;

    for (int k = 0; k < N; ++k) {
        sum += A[row * N + k] * B[k * N + col];
    }

    C[row * N + col] = sum;
}


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
    int i = get_global_id(0);
    if (i >= n_bodies) {
        return;
    }

    accX[i] = 0.f;
    accY[i] = 0.f;

    float dx, dy, distSqr, dist, invDistCubed, f;

    for (int j = 0; j < n_bodies; ++j) {
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
    int i = get_global_id(0);
    if (i >= n_bodies) {
        return;
    }

    velX[i] += accX[i] * dt;
    velY[i] += accY[i] * dt;

    posX[i] += velX[i] * dt;
    posY[i] += velY[i] * dt;

    posX[i] = fmod(posX[i] + WIDTH, WIDTH);
    posY[i] = fmod(posY[i] + HEIGHT, HEIGHT);
}
