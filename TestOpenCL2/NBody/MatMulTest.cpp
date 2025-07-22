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


constexpr size_t n_bodies = 1632; // hier ändern

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
    std::vector<float> A(n_bodies * n_bodies, 1.0f), B(n_bodies * n_bodies, 2.0f), C(n_bodies * n_bodies, 0.0f), C_scalar(n_bodies * n_bodies, 0.0f);

    cl_int err;

    cl_uint platformCount;
    cl_platform_id platform;
    err = clGetPlatformIDs(1, &platform, &platformCount);
    checkError(err, "clGetPlatformIDs");

    cl_uint deviceCount;
    cl_device_id device;
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 1, &device, &deviceCount);
    checkError(err, "clGetDeviceIDs");

    cl_context context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    checkError(err, "clCreateContext");

    cl_command_queue queue = clCreateCommandQueue(context, device, 0, &err);
    checkError(err, "clCreateCommandQueue");

    cl_mem bufferA = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * A.size(), A.data(), &err);
    checkError(err, "clCreateBuffer (bufferA)");
    cl_mem bufferB = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * B.size(), B.data(), &err);
    checkError(err, "clCreateBuffer (bufferB)");
    cl_mem bufferC = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(float) * C.size(), NULL, &err);
    checkError(err, "clCreateBuffer (bufferC)");

    std::string sourceStr = readKernelSource("../opencl/NBody.cl");
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

    cl_kernel matrix_multiply_kernel = clCreateKernel(program, "matrix_multiply", &err);
    checkError(err, "clCreateKernel");

    size_t global_work_size[2] = { n_bodies, n_bodies };
    size_t local_work_size[2] = { 16, 16 };


    // Haupt-Loop
    while (true) {
        //! Set kernel arguments for compute_forces
        err = clSetKernelArg(matrix_multiply_kernel, 0, sizeof(cl_mem), &bufferA);
        checkError(err, "clSetKernelArg (bufferA)");
        err = clSetKernelArg(matrix_multiply_kernel, 1, sizeof(cl_mem), &bufferB);
        checkError(err, "clSetKernelArg (bufferB)");
        err = clSetKernelArg(matrix_multiply_kernel, 2, sizeof(cl_mem), &bufferC);
        checkError(err, "clSetKernelArg (bufferC)");
        err = clSetKernelArg(matrix_multiply_kernel, 3, sizeof(int), &n_bodies);
        checkError(err, "clSetKernelArg (N)");

        //! Call kernels
        //TODO
        err = clEnqueueNDRangeKernel(queue, matrix_multiply_kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
        checkError(err, "clEnqueueNDRangeKernel");

        clFinish(queue);

        //! Read results back
        err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data(), 0, NULL, NULL);
        checkError(err, "clEnqueueReadBuffer");
        //! Draw bodies from SOA
        std::cout << "done" << std::endl;
    }

    //! Cleanup OpenCL resources
    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(matrix_multiply_kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    return 0;
}