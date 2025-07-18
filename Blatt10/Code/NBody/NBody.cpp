#include <cmath>
#include "include/Body.h"
#include "include/BodiesSOA.h"
#include <fstream>
#include <sstream>

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
constexpr size_t n_bodies = 100; /// hier ändern
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

void initialize_bodies_soa(BodiesSOA& bodies, const size_t n_bodies, float center_mass, int width, int height) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(50.0f, std::min(width, height) / 2.f - 20.f);
    std::uniform_real_distribution<float> mass_dist(0.5f, 10.f);

    // Körper 0 im Zentrum
    bodies.posX[0] = 0.f;
    bodies.posY[0] = 0.f;
    bodies.velX[0] = 0.f;
    bodies.velY[0] = 0.f;
    bodies.accX[0] = 0.f;
    bodies.accY[0] = 0.f;
    bodies.mass[0] = center_mass;

    for (size_t i = 1; i < n_bodies; ++i) {
        float angle = angle_dist(rng);
        float r = radius_dist(rng);
        float mass = mass_dist(rng);

        float x = r * std::cos(angle);
        float y = r * std::sin(angle);

        float v = orbital_velocity_scalar(center_mass, r);
        float vx = -v * std::sin(angle);
        float vy = v * std::cos(angle);

        bodies.posX[i] = x;
        bodies.posY[i] = y;
        bodies.velX[i] = vx;
        bodies.velY[i] = vy;
        bodies.accX[i] = 0.f;
        bodies.accY[i] = 0.f;
        bodies.mass[i] = mass;
    }
}


void checkError(cl_int err, const char* operation) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error during operation '" << operation << "': " << err << std::endl;
        exit(1);
    }
}

std::string readKernelSource(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void init_opencl(
    size_t n,
    size_t* global_work_size,
    size_t* local_work_size,
    BodiesSOA& bodies,
    cl_context& context,
    cl_command_queue& queue,
    cl_program& program,
    cl_kernel& compute_forces_kernel,
    cl_kernel& integrate_bodies_kernel,
    cl_mem& posX_buf,
    cl_mem& posY_buf,
    cl_mem& accX_buf,
    cl_mem& accY_buf,
    cl_mem& velX_buf,
    cl_mem& velY_buf,
    cl_mem& mass_buf){

        cl_int err;
        cl_uint platformCount;
        cl_platform_id platform;
        err = clGetPlatformIDs(1, &platform, &platformCount);
        checkError(err, "clGetPlatformIDs");

        cl_uint deviceCount;
        cl_device_id device;
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
        checkError(err, "clGetDeviceIDs");

        // set a context with in which the program will work
        context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        checkError(err, "clCreateContext");

        // create a command-queue
        queue = clCreateCommandQueue(context, device, 0, &err);
        checkError(err, "clCreateCommandQueue");

        // create memorybuffer(s)
        posX_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.posX.data(), &err);
        checkError(err, "clCreateBuffer (posX_buf)");
        posY_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.posY.data(), &err);
        checkError(err, "clCreateBuffer (posY_buf)");
        velX_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.velX.data(), &err);
        checkError(err, "clCreateBuffer (velX_buf)");
        velY_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.velY.data(), &err);
        checkError(err, "clCreateBuffer (velY_buf)");
        accX_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.accX.data(), &err);
        checkError(err, "clCreateBuffer (accX_buf)");
        accY_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.accY.data(), &err);
        checkError(err, "clCreateBuffer (accY_buf)");
        mass_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n, bodies.mass.data(), &err);
        checkError(err, "clCreateBuffer (mass_buf)");
        

        
        // load OpenCl-Kernel
        std::string sourceStr = readKernelSource("NBody.cl");
        const char* source = sourceStr.c_str();
        program = clCreateProgramWithSource(context, 1, &source, NULL, &err);
        checkError(err, "clCreateProgramWithSource");
        
        // translate for device
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

        global_work_size[0] =  n_bodies ;
        local_work_size[0] =  16 ;

        // create a kernel
        compute_forces_kernel = clCreateKernel(program, "compute_forces", &err);
        checkError(err, "clCreateComputeForcesKernel");
        integrate_bodies_kernel = clCreateKernel(program, "integrate_bodies", &err);
        checkError(err, "clCreateIntegrateBodiesKernel");
}


void run_opencl(
    BodiesSOA& bodies,
    cl_mem& posX_buf, 
    cl_mem& posY_buf, 
    cl_mem& accX_buf, 
    cl_mem& accY_buf, 
    cl_mem& velX_buf,
    cl_mem& velY_buf,
    cl_mem& mass_buf,
    const float& dt,
    const int& n,
    const float& G,
    const float& eps,
    cl_command_queue& queue,
    cl_kernel& compute_forces_kernel,
    cl_kernel& integrate_bodies_kernel,
    const size_t* global_work_size, 
    const size_t* local_work_size,
    cl_int& err){
    // compute_forces Kernel-Argumente setzen
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
    err = clSetKernelArg(compute_forces_kernel, 5, sizeof(int), &n);
    checkError(err, "clSetKernelArg (n)");
    err = clSetKernelArg(compute_forces_kernel, 6, sizeof(float), &G);
    checkError(err, "clSetKernelArg (G)");
    err = clSetKernelArg(compute_forces_kernel, 7, sizeof(float), &eps);
    checkError(err, "clSetKernelArg (eps)");

    // integrate_bodies Kernel-Argumente setzen
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
    err = clSetKernelArg(integrate_bodies_kernel, 6, sizeof(float), &dt);
    checkError(err, "clSetKernelArg (dt_buf)");

    // call the kernel
    err = clEnqueueNDRangeKernel(queue, compute_forces_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    // call the kernel
    err = clEnqueueNDRangeKernel(queue, integrate_bodies_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    // wait for queue until it's finish
    clFinish(queue);


    // read the buffer
    err = clEnqueueReadBuffer(queue, posX_buf, CL_TRUE, 0, sizeof(float) * n, bodies.posX.data(), 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer posX");

    err = clEnqueueReadBuffer(queue, posY_buf, CL_TRUE, 0, sizeof(float) * n, bodies.posX.data(), 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer posY");

}


void cleanup_opencl(
    cl_mem& posX_buf, 
    cl_mem& posY_buf, 
    cl_mem& accX_buf, 
    cl_mem& accY_buf, 
    cl_mem& velX_buf,
    cl_mem& velY_buf,
    cl_mem& mass_buf,
    const cl_kernel& integrate_bodies_kernel,
    const cl_kernel& compute_forces_kernel,
    const cl_program& program,
    const cl_command_queue& queue,
    const cl_context& context){

    // clean the memory
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

int main() {

    //BodySOA

    BodiesSOA bodiesSoa(n_bodies);
    initialize_bodies_soa(bodiesSoa, n_bodies, center_mass, WIDTH, HEIGHT);
    

    size_t global_work_size[1];
    size_t local_work_size[1];
    


    // OpenCL-Objekte
    cl_context context;
    cl_command_queue queue;
    cl_program program;
    cl_kernel compute_forces_kernel, integrate_bodies_kernel;
    cl_mem posX_buf, posY_buf, accX_buf, accY_buf, velX_buf, velY_buf, mass_buf;
    cl_int err;

    // Nur 1x Aufruf!
    init_opencl(n_bodies, global_work_size, local_work_size, bodiesSoa,
                context, queue, program,
                compute_forces_kernel, integrate_bodies_kernel,
                posX_buf, posY_buf, accX_buf, accY_buf,
                velX_buf, velY_buf, mass_buf);


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
    // initialize_bodies(bodies, n_bodies, center_mass, WIDTH, HEIGHT);

    // Uhr zur FPS-Berechnung
    sf::Clock frameClock;
    sf::Clock fpsClock;
    float lastFPS = 0.f;

    // Haupt-Loop
    while (window.isOpen()) {
        sf::Event e;
        while (window.pollEvent(e)) if (e.type == sf::Event::Closed) window.close();

        // scalar version
        // compute_forces(bodies, G, eps, n_bodies);
        // integrate_bodies(bodies, dt, n_bodies);

        run_opencl(bodiesSoa,
                   posX_buf, posY_buf, accX_buf, accY_buf, velX_buf, velY_buf, mass_buf,
                   dt, n_bodies, G, eps,
                   queue, compute_forces_kernel, integrate_bodies_kernel,
                   global_work_size, local_work_size, err);

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
