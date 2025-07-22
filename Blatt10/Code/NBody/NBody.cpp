#define CL_TARGET_OPENCL_VERSION 120
#include "include/Body.h"
#include "include/BodiesSOA.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm> // für std::min
#include <random> // um Bodies zu initialisieren
#include <SFML/Graphics.hpp>
#include <gegl-0.4/opencl/cl.h>


size_t n_bodies{100}; // hier ändern
constexpr bool use_scalar_version{false}; // hier ändern 

constexpr float TARGET_FPS{165.f};
const sf::Time FRAME_DURATION = sf::seconds(1.f / TARGET_FPS);

// Fenstergröße
constexpr int WIDTH{1980};
constexpr int HEIGHT{1080};


constexpr float G{1.f};
constexpr float dt{.1f};
constexpr float eps{1e-1f};
constexpr float center_mass{1000.f};


void compute_forces(std::vector<Body>& bodies, const float G, const float eps) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].accX = 0.f;
        bodies[i].accY = 0.f;
    }

    for (size_t i = 0; i < bodies.size(); ++i) {
        for (size_t j = 0; j < bodies.size(); ++j) {

            if (i != j) {
                // 1) Berechne die Distanzen in x- und y-Richtung zum anderen Körper
                const float dx{bodies[j].posX - bodies[i].posX};
                const float dy{bodies[j].posY - bodies[i].posY};
                
                // 2) Berechne die kubisch inverse Distanz unter Berücksichtigung des Softening-Faktors eps
                const float distSqr{dx * dx + dy * dy + eps * eps};
                const float dist{std::sqrt(distSqr)};
                const float invDistCubed{1.f / (dist * dist * dist)};
                
                // 3) Berechne die Kraft, die von Körper Bj auf Körper Bi wirkt
                const float f{G * bodies[j].mass * invDistCubed};

                // 4) Summiere und aktualisiere die Beschleunigung in beide Richtungen für Körper Bi
                bodies[i].accX += dx * f;
                bodies[i].accY += dy * f;
            }
        }
    }
}

void integrate_bodies(std::vector<Body>& bodies, const float dt) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        // Geschwindigkeit aktualisieren
        bodies[i].velX += bodies[i].accX * dt;
        bodies[i].velY += bodies[i].accY * dt;

        // Position aktualisieren
        bodies[i].posX += bodies[i].velX * dt;
        bodies[i].posY += bodies[i].velY * dt;

        // Toroidales Verhalten
        bodies[i].posX = std::fmod(bodies[i].posX + WIDTH, WIDTH);
        bodies[i].posY = std::fmod(bodies[i].posY + HEIGHT, HEIGHT);
    }
}

sf::Color mass_to_color(const float m) {
    const float norm{std::min(1.0f, m / 10.0f)};
    return sf::Color(255 * norm, 50, 255 * (1 - norm));
}

float orbital_velocity_scalar(const float M, const float r) {
    return std::sqrt(1.0f * M / r); // G = 1.0 assumed
}

void initialize_bodies(std::vector<Body>& bodies, const size_t n_bodies, const float center_mass, const int width, const int height) {
    std::mt19937 rng{42};
    std::uniform_real_distribution<float> angle_dist{0.0f, 2.0f * M_PI};
    std::uniform_real_distribution<float> radius_dist{50.0f, std::min(width, height) / 2.f - 20.f};
    std::uniform_real_distribution<float> mass_dist{0.5f, 10.f};

    bodies.reserve(n_bodies);

    // Schwerer Körper im Zentrum
    bodies.emplace_back(0.f, 0.f, 0.f, 0.f, center_mass);

    // Zufällige Körper
    for (size_t i = 1; i < n_bodies; ++i) {
        const float angle{angle_dist(rng)};
        const float r{radius_dist(rng)};
        const float mass{mass_dist(rng)};

        const float x{r * std::cos(angle)};
        const float y{r * std::sin(angle)};

        // Kreisbahn-Geschwindigkeit
        const float v{orbital_velocity_scalar(center_mass, r)};
        const float vx{-v * std::sin(angle)};
        const float vy{v * std::cos(angle)};

        bodies.emplace_back(x, y, vx, vy, mass);
    }
}

void initialize_bodies_soa(BodiesSOA& bodies_soa, const size_t n_bodies, const float center_mass, const int width, const int height) {
    std::mt19937 rng{42};
    std::uniform_real_distribution<float> angle_dist{0.0f, 2.0f * M_PI};
    std::uniform_real_distribution<float> radius_dist{50.0f, std::min(width, height) / 2.f - 20.f};
    std::uniform_real_distribution<float> mass_dist{0.5f, 10.f};

    bodies_soa.reserve(n_bodies);

    // Schwerer Körper im Zentrum
    bodies_soa.add_body(0.f, 0.f, 0.f, 0.f, center_mass);

    // Zufällige Körper
    for (size_t i = 1; i < n_bodies; ++i) {
        const float angle{angle_dist(rng)};
        const float r{radius_dist(rng)};
        const float mass{mass_dist(rng)};

        const float x{r * std::cos(angle)};
        const float y{r * std::sin(angle)};

        // Kreisbahn-Geschwindigkeit
        const float v{orbital_velocity_scalar(center_mass, r)};
        const float vx{-v * std::sin(angle)};
        const float vy{v * std::cos(angle)};

        bodies_soa.add_body(x, y, vx, vy, mass);
    }
}

std::string readKernelSource(const char* filename) {
    std::ifstream file{filename};
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


void checkError(const cl_int err, const char* operation) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error during operation '" << operation << "': " << err << std::endl;
        exit(1);
    }
}

int main() {
    // Fenster erstellen
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation");

    // Schriftart laden
    sf::Font font;
    if (!font.loadFromFile("../OpenSans-Bold.ttf")) {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    // FPS Text vorbereiten
    sf::Text fpsText("", font, 18);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, 5);

    // Uhr zur FPS-Berechnung
    sf::Clock frameClock;
    sf::Clock fpsClock;
    float lastFPS = 0.f;

    if (!use_scalar_version) {
        // Nächstgrößere Zahl, die durch 16 teilbar ist
        n_bodies = ((n_bodies + static_cast<size_t>(15)) / static_cast<size_t>(16)) * static_cast<size_t>(16); 
    }
    const int N = static_cast<int>(n_bodies);

    cl_int err;
    cl_uint platformCount;
    cl_platform_id platform;
    cl_uint deviceCount;
    cl_device_id device;
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel compute_forces_kernel, integrate_bodies_kernel;
    cl_mem posX_buf, posY_buf, accX_buf, accY_buf, velX_buf, velY_buf, mass_buf; //! vllt ändern
    const size_t global_work_size[1] = {N};
    const size_t local_work_size[1] = {16};

    std::vector<Body> bodies;
    BodiesSOA bodies_soa;

    // Körper initialisieren
    if (use_scalar_version) {
        initialize_bodies(bodies, n_bodies, center_mass, WIDTH, HEIGHT);
    }
    else {
        initialize_bodies_soa(bodies_soa, n_bodies, center_mass, WIDTH, HEIGHT);

        err = clGetPlatformIDs(1, &platform, &platformCount);
        checkError(err, "clGetPlatformIDs");

        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
        checkError(err, "clGetDeviceIDs");

        context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        checkError(err, "clCreateContext");

        queue = clCreateCommandQueue(context, device, 0, &err);
        checkError(err, "clCreateCommandQueue");

        posX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.posX.data(), &err);
        checkError(err, "clCreateBuffer (posX_buf)");
        posY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.posY.data(), &err);
        checkError(err, "clCreateBuffer (posY_buf)");
        velX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.velX.data(), &err);
        checkError(err, "clCreateBuffer (velX_buf)");
        velY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.velY.data(), &err);
        checkError(err, "clCreateBuffer (velY_buf)");
        accX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.accX.data(), &err);
        checkError(err, "clCreateBuffer (accX_buf)");
        accY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.accY.data(), &err);
        checkError(err, "clCreateBuffer (accY_buf)");
        mass_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * N, bodies_soa.mass.data(), &err);
        checkError(err, "clCreateBuffer (mass_buf)");

        const std::string sourceStr = readKernelSource("../opencl/NBody.cl");
        const char* source = sourceStr.c_str();
        program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
        checkError(err, "clCreateProgramWithSource");

        err = clBuildProgram(program, 1, &device, NULL, NULL, NULL);
        if (err != CL_SUCCESS) {
            size_t log_size;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);
            std::vector<char> log(log_size);
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG, log_size, log.data(), NULL);
            std::cerr << "Error during operation 'clBuildProgram': " << err << std::endl;
            std::cerr << "Build log:" << std::endl << log.data() << std::endl;
            exit(1);
        }

        compute_forces_kernel = clCreateKernel(program, "compute_forces", &err);
        checkError(err, "clCreateKernel (compute_forces)");
        integrate_bodies_kernel = clCreateKernel(program, "integrate_bodies", &err);
        checkError(err, "clCreateKernel (integrate_bodies)");

    }


    // Haupt-Loop
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        if (use_scalar_version) {
            compute_forces(bodies, G, eps);
            integrate_bodies(bodies, dt);

            window.clear(sf::Color::Black);

            for (const Body& b : bodies) {
                sf::CircleShape circle(b.mass > 50.0f ? 6 : 2);
                circle.setFillColor(mass_to_color(b.mass));
                circle.setPosition(WIDTH / 2 + b.posX, HEIGHT / 2 + b.posY);
                circle.setOrigin(circle.getRadius(), circle.getRadius());
                window.draw(circle);
            }
        }
        else {
            //! Set kernel arguments for compute_forces
            err = clSetKernelArg(compute_forces_kernel, 0, sizeof(cl_mem), &posX_buf);
            checkError(err, "clSetKernelArg (posX_buf)");
            err = clSetKernelArg(compute_forces_kernel, 1, sizeof(cl_mem), &posY_buf);
            checkError(err, "clSetKernelArg (posY_buf)");
            err = clSetKernelArg(compute_forces_kernel, 2, sizeof(cl_mem), &accX_buf);
            checkError(err, "clSetKernelArg (accX_buf)");
            err = clSetKernelArg(compute_forces_kernel, 3, sizeof(cl_mem), &accY_buf);
            checkError(err, "clSetKernelArg (accY_buf)");
            err = clSetKernelArg(compute_forces_kernel, 4, sizeof(cl_mem), &mass_buf);
            checkError(err, "clSetKernelArg (mass_buf)");
            err = clSetKernelArg(compute_forces_kernel, 5, sizeof(int), &N);
            checkError(err, "clSetKernelArg (N)");
            err = clSetKernelArg(compute_forces_kernel, 6, sizeof(float), &G);
            checkError(err, "clSetKernelArg (G)");
            err = clSetKernelArg(compute_forces_kernel, 7, sizeof(float), &eps);
            checkError(err, "clSetKernelArg (eps)");

            //! Set kernel arguments for integrate_bodies
            err = clSetKernelArg(integrate_bodies_kernel, 0, sizeof(cl_mem), &posX_buf);
            checkError(err, "clSetKernelArg (posX_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 1, sizeof(cl_mem), &posY_buf);
            checkError(err, "clSetKernelArg (posY_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 2, sizeof(cl_mem), &velX_buf);
            checkError(err, "clSetKernelArg (velX_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 3, sizeof(cl_mem), &velY_buf);
            checkError(err, "clSetKernelArg (velY_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 4, sizeof(cl_mem), &accX_buf);
            checkError(err, "clSetKernelArg (accX_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 5, sizeof(cl_mem), &accY_buf);
            checkError(err, "clSetKernelArg (accY_buf)");
            err = clSetKernelArg(integrate_bodies_kernel, 6, sizeof(float), &N);
            checkError(err, "clSetKernelArg (N)");
            err = clSetKernelArg(integrate_bodies_kernel, 7, sizeof(float), &dt);
            checkError(err, "clSetKernelArg (dt)");
            err = clSetKernelArg(integrate_bodies_kernel, 8, sizeof(float), &WIDTH);
            checkError(err, "clSetKernelArg (WIDTH)");
            err = clSetKernelArg(integrate_bodies_kernel, 9, sizeof(float), &HEIGHT);
            checkError(err, "clSetKernelArg (HEIGHT)");

            //! Call kernels
            err = clEnqueueNDRangeKernel(queue, compute_forces_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
            checkError(err, "clEnqueueNDRangeKernel (compute_forces)");
            clFinish(queue);
            err = clEnqueueNDRangeKernel(queue, integrate_bodies_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
            checkError(err, "clEnqueueNDRangeKernel (integrate_bodies)");
            clFinish(queue);

            //! Read results back
            err = clEnqueueReadBuffer(queue, posX_buf, CL_TRUE, 0, sizeof(float) * N, bodies_soa.posX.data(), 0, NULL, NULL);
            checkError(err, "clEnqueueReadBuffer posX");
            err = clEnqueueReadBuffer(queue, posY_buf, CL_TRUE, 0, sizeof(float) * N, bodies_soa.posY.data(), 0, NULL, NULL);
            checkError(err, "clEnqueueReadBuffer posY");

            window.clear(sf::Color::Black);

            //! Draw bodies from SOA
            for (size_t i = 0; i < N; ++i) {
                sf::CircleShape circle(bodies_soa.mass[i] > 50.0f ? 6 : 2);
                circle.setFillColor(mass_to_color(bodies_soa.mass[i]));
                circle.setPosition(WIDTH / 2 + bodies_soa.posX[i], HEIGHT / 2 + bodies_soa.posY[i]);
                circle.setOrigin(circle.getRadius(), circle.getRadius());
                window.draw(circle);
            }
        }

        //! FPS calculation and display
        float fpsElapsed = fpsClock.restart().asSeconds();
        lastFPS = 1.0f / fpsElapsed;
        fpsText.setString("FPS: " + std::to_string((int)lastFPS));
        window.draw(fpsText);
    
        window.display();
    
        sf::Time frameElapsed = frameClock.getElapsedTime();
        if (frameElapsed < FRAME_DURATION) sf::sleep(FRAME_DURATION - frameElapsed);
        frameClock.restart();
    }

    //! Cleanup OpenCL resources
    if (!use_scalar_version) {
        clReleaseMemObject(posX_buf);
        clReleaseMemObject(posY_buf);
        clReleaseMemObject(accX_buf);
        clReleaseMemObject(accY_buf);
        clReleaseMemObject(velX_buf);
        clReleaseMemObject(velY_buf);
        clReleaseMemObject(mass_buf);

        clReleaseKernel(integrate_bodies_kernel);
        clReleaseKernel(compute_forces_kernel);
        clReleaseProgram(program);
        clReleaseCommandQueue(queue);
        clReleaseContext(context);
    }

    return 0;
}