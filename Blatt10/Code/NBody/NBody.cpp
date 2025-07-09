#include <cmath>
#include "include/Body.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <random> // um Bodies zu initialisieren




/*
* @author R. Lakos, A. Mithran, O. Tyagi
* @date 2024-06-14
* To compile your source code, please use the following command to link the OpenCL library: 
* g++ MatAdd.cpp -o MatAdd -O3 -fno-tree-vectorize -I/usr/include/gegl-0.4 -L/usr/lib64 /usr/lib64/libOpenCL.so.1
*/

#define CL_TARGET_OPENCL_VERSION 120

#include <gegl-0.4/opencl/cl.h>




constexpr float G = 1.f;
constexpr float dt = .1f;
constexpr float eps = 1e-1f;
constexpr size_t n_bodies = 1000; /// hier ändern
constexpr float center_mass = 1000.f;


constexpr float TARGET_FPS = 165.f;
const sf::Time FRAME_DURATION = sf::seconds(1.f / TARGET_FPS);

// Fenstergröße
constexpr int WIDTH = 1980, HEIGHT = 1080;
// Fenster erstellen
sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "N-Body Simulation");

// Updates für Bodies
void compute_forces(std::vector<Body>& bodies, const float G, const float eps, const size_t n_bodies) {
    for (size_t i = 0; i < bodies.size(); ++i) {
        bodies[i].accX = 0.f;
        bodies[i].accY = 0.f;
    }
    float dx, dy, distSqr, dist, invDistCubed, f;

    for (size_t i = 0; i < n_bodies; ++i) {
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
}

void integrate_bodies(std::vector<Body>& bodies, const float dt, const size_t n_bodies) {
    for (size_t i = 0; i < n_bodies; ++i) {
        // Geschwindigkeit aktualisieren
        bodies[i].velX += bodies[i].accX * dt;
        bodies[i].velY += bodies[i].accY * dt;

        // Position aktualisieren
        bodies[i].posX += bodies[i].velX * dt;
        bodies[i].posY += bodies[i].velY * dt;
    }
}

sf::Color mass_to_color(const float m) {
    float norm = std::min(1.0f, m / 10.0f);
    return sf::Color(255 * norm, 50, 255 * (1 - norm));
}

float orbital_velocity_scalar(const float M, const float r) {
    return std::sqrt(1.0f * M / r); // G = 1.0 assumed
}


void initialize_bodies(std::vector<Body>& bodies, const size_t n_bodies, const float center_mass, const int width, const int height) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(50.0f, std::min(width, height) / 2.f - 20.f);
    std::uniform_real_distribution<float> mass_dist(0.5f, 10.f);

    // Schwerer Körper im Zentrum
    bodies.push_back(Body{0.f, 0.f, 0.f, 0.f, center_mass});

    // Zufällige Körper
    float angle,r,mass,x,y,v,vx,vy;
    
    for (size_t i = 1; i < n_bodies; ++i) {
        angle = angle_dist(rng);
        r = radius_dist(rng);
        mass = mass_dist(rng);

        x = r * std::cos(angle);
        y = r * std::sin(angle);

        // Kreisbahn-Geschwindigkeit
        v = orbital_velocity_scalar(center_mass, r);
        vx = -v * std::sin(angle);
        vy = v * std::cos(angle);

        bodies.push_back(Body{x, y, vx, vy, mass});
    }
}

void init_opencl(){
    cl_int err;

    // const int N = 2048;
    // float tolerance = 1e-6;

    // std::vector<float> A(N * N, 1.0f), B(N * N, 2.0f), C(N * N, 0.0f), C_scalar(N * N, 0.0f);

    // auto start = Clock_t::now();

    // get a platform / device - the information about thewhole / select the device forcomputations
    cl_uint platformCount;
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, &platformCount);
    checkError(err, "clGetPlatformIDs");

    cl_uint deviceCount;
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
    checkError(err, "clGetDeviceIDs");

    // set a contextwithinwhich the programwillwork
    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    // create a command-queue
    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    // create memorybuffer(s)
    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * A.size(), A.data(), &err);
    checkError(err, "clCreateBuffer (bufferA)");
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * B.size(), B.data(), &err);
    checkError(err, "clCreateBuffer (bufferB)");
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * C.size(), NULL, &err);
    checkError(err, "clCreateBuffer (bufferC)");


    std::string sourceStr = readKernelSource("matrix_addition.cl");
    const char* source = sourceStr.c_str();
    cl_program program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
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

    // create a kernel
    cl_kernel kernel = clCreateKernel(program, "matrix_add", &err);
    checkError(err, "clCreateKernel");

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    checkError(err, "clSetKernelArg (bufferA)");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    checkError(err, "clSetKernelArg (bufferB)");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    checkError(err, "clSetKernelArg (bufferC)");
    err = clSetKernelArg(kernel, 3, sizeof(int), &N);
    checkError(err, "clSetKernelArg (N)");

    size_t global_work_size[1] = { (size_t)N};
    size_t local_work_size[1] = { 16 };

    // call the kernel
    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    // read the buffer
    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data(), 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer");

    // auto finish = Clock_t::now();
    // std::chrono::duration<double> elapsed = finish - start;
    // std::cout << "OpenCL - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    // clean the memory
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    // start = Clock_t::now();
    // ScalarMatrixAddition(A, B, C_scalar, N);
    // finish = Clock_t::now();
    // elapsed = finish - start;
    // std::cout << "Scalar - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    // bool isSame = CompareMatrices(C, C_scalar, tolerance, N);
    // if (isSame) {
    //     std::cout << "The matrices are the same within tolerance." << std::endl;
    // } else {
    //     std::cout << "The matrices are different." << std::endl;
    // }

}

int main() {
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

    // Körper initialisieren
    std::vector<Body> bodies;
    initialize_bodies(bodies, n_bodies, center_mass, WIDTH, HEIGHT);

    // Uhr zur FPS-Berechnung
    sf::Clock frameClock;
    sf::Clock fpsClock;
    float lastFPS = 0.f;

    // Haupt-Loop
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        compute_forces(bodies, G, eps, n_bodies);
        integrate_bodies(bodies, dt, n_bodies);

        window.clear(sf::Color::Black);


        for (const Body& b : bodies) {
            sf::CircleShape circle(b.mass > 50.0f ? 6 : 2);
            circle.setFillColor(mass_to_color(b.mass));
            circle.setPosition(WIDTH / 2 + b.posX, HEIGHT / 2 + b.posY);
            circle.setOrigin(circle.getRadius(), circle.getRadius());
            window.draw(circle);
        }

        // FPS calculation and display
        float fpsElapsed = fpsClock.restart().asSeconds();
        lastFPS = 1.0f / fpsElapsed;
        fpsText.setString("FPS: " + std::to_string((int)lastFPS));
        window.draw(fpsText);
    
        window.display();
    
        sf::Time frameElapsed = frameClock.getElapsedTime();
        if (frameElapsed < FRAME_DURATION) sf::sleep(FRAME_DURATION - frameElapsed);
        frameClock.restart();
    }

    return 0;
}
