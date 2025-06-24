/*
* @author R. Lakos, A. Mithran, O. Tyagi
* @date 2024-06-14
* To compile your source code, please use the following command to link the OpenCL library: 
* g++ MatMul.cpp -o MatMul -O3 -fno-tree-vectorize -I/usr/include/gegl-0.4 -L/usr/lib64 /usr/lib64/libOpenCL.so.1
*/

// g++ MatMul.cpp -o MatMulFast -O3 -fno-tree-vectorize -I/usr/include/gegl-0.4 -L/usr/lib64 /usr/lib64/libOpenCL.so.1 -msse -fopenmp -I~/Vc/include -L~/Vc/lib ~/Vc/lib/libVc.a
//! noch rein

#define CL_TARGET_OPENCL_VERSION 120

#include <gegl-0.4/opencl/cl.h>

#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <omp.h>
#include "Vc/Vc/Vc"
#include <cstdlib>


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

// diese Definition hinzugefügt
bool CompareMatrices(float* mat1, const std::vector<float>& mat2, float tolerance, int N) {
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

void FastMatrixMultiplication(float* a, float* b_T, float* c, int N) {
    // b_T ist eine Matrix b, die schon transponiert ist. Die Transponierung ist hilfreich für reinterpret_cast.
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            Vc::float_v sum = 0.0f; // Dieser Vektor wird die partiellen Summen beinhalten
            for (int k = 0; k < N; k += Vc::float_v::Size) {
                Vc::float_v& aVec = reinterpret_cast<Vc::float_v&>(a[i * N + k]);
                Vc::float_v& bVec = reinterpret_cast<Vc::float_v&>(b_T[j * N + k]);
                sum += aVec * bVec; // Es wird "Zeile mal Zeile" gerechnet, da b transponiert ist.
            }
            for (int m = 0; m < Vc::float_v::Size; m++) {
                c[i * N + j] += sum[m]; // die partiellen Summen werden zusammenaddiert
            }
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

    const int N = 16384;
    std::cout << "N = " << N << ":\n" << std::endl;
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

    if (N <= 4096) {
        start = Clock_t::now();
        ScalarMatrixMultiplication(A, B, C_scalar, N);
        finish = Clock_t::now();
        elapsed = finish - start;
        std::cout << "Scalar - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;
    }
    else {
        std::cout << "N too large for scalar version." << std::endl;
    }

    // Datenstrukturen mit besserer Speicherallokierung und -ausrichtung
    float* A_omp = static_cast<float*>(std::aligned_alloc(16, N * N * sizeof(float)));
    float* B_omp = static_cast<float*>(std::aligned_alloc(16, N * N * sizeof(float)));
    float* C_omp = static_cast<float*>(std::aligned_alloc(16, N * N * sizeof(float)));

    for (size_t i = 0; i < N * N; i++) {
        A_omp[i] = 1.0f;
        B_omp[i] = 2.0f; // Bemerkung: B_omp transponiert = B_omp
        C_omp[i] = 0.0f;
    }

    start = Clock_t::now();
    FastMatrixMultiplication(A_omp, B_omp, C_omp, N);
    finish = Clock_t::now();
    elapsed = finish - start;
    std::cout << "OpenMP + SIMD - elapsed time: " << std::chrono::duration_cast<TimeUnit_t>(elapsed).count() << time_units << std::endl;

    if (N <= 4096) {
        bool isSame = CompareMatrices(C, C_scalar, tolerance, N);
        if (isSame) {
            std::cout << "OpenCL and scalar matrices are the same within tolerance." << std::endl;
        } else {
            std::cout << "OpenCL and scalar matrices are different." << std::endl;
        }
    
        isSame = CompareMatrices(C_omp, C_scalar, tolerance, N);
        if (isSame) {
            std::cout << "OpenMP and scalar matrices are the same within tolerance." << std::endl;
        } else {
            std::cout << "OpenMP and scalar matrices different." << std::endl;
        }
    }
    else {
        bool isSame = CompareMatrices(C_omp, C, tolerance, N);
        if (isSame) {
            std::cout << "OpenMP and OpenCL matrices are the same within tolerance." << std::endl;
        } else {
            std::cout << "OpenMP and OpenCL matrices are different." << std::endl;
        }
    }

    return 0;
}
