#include "include/BodiesSOA.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <algorithm> // für std::min
#include <random> // um Bodies zu initialisieren
#include <immintrin.h> // für _mm_malloc/_mm_free

#define CL_TARGET_OPENCL_VERSION 120

#include <gegl-0.4/opencl/cl.h>


constexpr size_t n_bodies = 100; // hier ändern

constexpr float G = 1.f;
constexpr float dt = .1f;
constexpr float eps = 1e-1f;
constexpr float center_mass = 1000.f;

// Fenstergröße
constexpr int WIDTH = 1980, HEIGHT = 1080;

float orbital_velocity_scalar(const float M, const float r) {
    return std::sqrt(1.0f * M / r); // G = 1.0 assumed
}

void initialize_bodies_soa(BodiesSOA& bodies_soa, const size_t n_bodies, const float center_mass, const int width, const int height) {
    std::mt19937 rng(42);
    std::uniform_real_distribution<float> angle_dist(0.0f, 2.0f * M_PI);
    std::uniform_real_distribution<float> radius_dist(50.0f, std::min(width, height) / 2.f - 20.f);
    std::uniform_real_distribution<float> mass_dist(0.5f, 10.f);

    bodies_soa.reserve(n_bodies);

    // Schwerer Körper im Zentrum
    bodies_soa.add_body(0.f, 0.f, 0.f, 0.f, center_mass);

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

        bodies_soa.add_body(x, y, vx, vy, mass);
    }
}

std::string readKernelSource(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


void checkError(cl_int err, const char* operation) {
    if (err != CL_SUCCESS) {
        std::cerr << "Error during operation '" << operation << "': " << err << std::endl;
        exit(1);
    }
}

int main() {
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
    size_t global_work_size[1] = {n_bodies};
    size_t local_work_size[1] = {16};
    
    BodiesSOA bodies_soa;

    // Körper initialisieren
    initialize_bodies_soa(bodies_soa, n_bodies, center_mass, WIDTH, HEIGHT);

    err = clGetPlatformIDs(1, &platform, &platformCount);
    checkError(err, "clGetPlatformIDs");

    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
    checkError(err, "clGetDeviceIDs");

    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    posX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.posX.data(), &err);
    checkError(err, "clCreateBuffer (posX_buf)");
    posY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.posY.data(), &err);
    checkError(err, "clCreateBuffer (posY_buf)");
    velX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.velX.data(), &err);
    checkError(err, "clCreateBuffer (velX_buf)");
    velY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.velY.data(), &err);
    checkError(err, "clCreateBuffer (velY_buf)");
    accX_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.accX.data(), &err);
    checkError(err, "clCreateBuffer (accX_buf)");
    accY_buf = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.accY.data(), &err);
    checkError(err, "clCreateBuffer (accY_buf)");
    mass_buf = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * n_bodies, bodies_soa.mass.data(), &err);
    checkError(err, "clCreateBuffer (mass_buf)");

    std::string sourceStr = readKernelSource("../opencl/NBody.cl");
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


    // Haupt-Loop
    while (true) {
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
        err = clSetKernelArg(compute_forces_kernel, 5, sizeof(int), &n_bodies);
        checkError(err, "clSetKernelArg (n_bodies)");
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
        err = clSetKernelArg(integrate_bodies_kernel, 6, sizeof(float), &n_bodies);
        checkError(err, "clSetKernelArg (n_bodies)");
        err = clSetKernelArg(integrate_bodies_kernel, 7, sizeof(float), &dt);
        checkError(err, "clSetKernelArg (dt)");
        err = clSetKernelArg(integrate_bodies_kernel, 8, sizeof(float), &WIDTH);
        checkError(err, "clSetKernelArg (WIDTH)");
        err = clSetKernelArg(integrate_bodies_kernel, 9, sizeof(float), &HEIGHT);
        checkError(err, "clSetKernelArg (HEIGHT)");

        //! Call kernels
        err = clEnqueueNDRangeKernel(queue, compute_forces_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
        checkError(err, "clEnqueueNDRangeKernel (compute_forces)");
        err = clEnqueueNDRangeKernel(queue, integrate_bodies_kernel, 1, NULL, global_work_size, local_work_size, 0, NULL, NULL);
        checkError(err, "clEnqueueNDRangeKernel (integrate_bodies)");

        clFinish(queue);

        //! Read results back
        err = clEnqueueReadBuffer(queue, posX_buf, CL_TRUE, 0, sizeof(float) * n_bodies, bodies_soa.posX.data(), 0, NULL, NULL);
        checkError(err, "clEnqueueReadBuffer posX");
        err = clEnqueueReadBuffer(queue, posY_buf, CL_TRUE, 0, sizeof(float) * n_bodies, bodies_soa.posY.data(), 0, NULL, NULL);
        checkError(err, "clEnqueueReadBuffer posY");

        //! Draw bodies from SOA
        std::cout << bodies_soa.posX[3] << std::endl;
    }

    //! Cleanup OpenCL resources
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

    return 0;
}