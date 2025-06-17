/*
* @author R. Lakos, A. Mithran, O. Tyagi
* @date 2024-06-14
* To compile your source code, please use the following command to link the OpenCL library: 
* g++ MatMul.cpp -o MatMul -O3 -fno-tree-vectorize -I/usr/include/gegl-0.4 -L/usr/lib64 /usr/lib64/libOpenCL.so.1
*/

#define CL_TARGET_OPENCL_VERSION 120

#include <gegl-0.4/opencl/cl.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>


using Clock_t = std::chrono::steady_clock;
using TimeUnit_t = std::chrono::milliseconds;


const std::string time_units = " ms";


bool CompareMatrices(const std::vector<float>& mat1, const std::vector<float>& mat2, float tolerance, int N) {
    for (int i = 0; i < N * N; ++i) {
        if (std::abs(mat1[i] - mat2[i]) > tolerance) {
            return false;
        }
    }
    return true;
}


void ScalarMatrixMultiplication(const std::vector<float>& A, const std::vector<float>& B, std::vector<float>& C, int N) {
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            float sum = 0.0f;
            for (int k = 0; k < N; ++k) {
                sum += A[i * N + k] * B[k * N + j];
            }
            C[i * N + j] = sum;
        }
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

    const int N = 2048;
    float tolerance = 1e-6;

    std::vector<float> A(N * N, 1.0f), B(N * N, 2.0f), C(N * N, 0.0f), C_scalar(N * N, 0.0f);

    auto start = Clock_t::now();

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

    std::string sourceStr = readKernelSource("matrix_multiplication.cl");
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

    cl_kernel kernel = clCreateKernel(program, "matrix_multiply", &err);
    checkError(err, "clCreateKernel");

    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &bufferA);
    checkError(err, "clSetKernelArg (bufferA)");
    err = clSetKernelArg(kernel, 1, sizeof(cl_mem), &bufferB);
    checkError(err, "clSetKernelArg (bufferB)");
    err = clSetKernelArg(kernel, 2, sizeof(cl_mem), &bufferC);
    checkError(err, "clSetKernelArg (bufferC)");
    err = clSetKernelArg(kernel, 3, sizeof(int), &N);
    checkError(err, "clSetKernelArg (N)");

    size_t global_work_size[2] = { (size_t)N, (size_t)N };
    size_t local_work_size[2] = { 16, 16 };

    err = clEnqueueNDRangeKernel(queue, kernel, 2, NULL, global_work_size, local_work_size, 0, NULL, NULL);
    checkError(err, "clEnqueueNDRangeKernel");

    err = clEnqueueReadBuffer(queue, bufferC, CL_TRUE, 0, sizeof(float) * C.size(), C.data(), 0, NULL, NULL);
    checkError(err, "clEnqueueReadBuffer");

    auto finish = Clock_t::now();
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "OpenCL - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    clReleaseMemObject(bufferA);
    clReleaseMemObject(bufferB);
    clReleaseMemObject(bufferC);
    clReleaseKernel(kernel);
    clReleaseProgram(program);
    clReleaseCommandQueue(queue);
    clReleaseContext(context);

    start = Clock_t::now();
    ScalarMatrixMultiplication(A, B, C_scalar, N);
    finish = Clock_t::now();
    elapsed = finish - start;
    std::cout << "Scalar - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    bool isSame = CompareMatrices(C, C_scalar, tolerance, N);
    if (isSame) {
        std::cout << "The matrices are the same within tolerance." << std::endl;
    } else {
        std::cout << "The matrices are different." << std::endl;
    }

    return 0;
}
