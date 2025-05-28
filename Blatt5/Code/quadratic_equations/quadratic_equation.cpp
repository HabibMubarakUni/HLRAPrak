/**
Authors: Robin Lakos (lakos@fias.uni-frankfurt.de)

Find the maximum root of a quadratic equation: a*x^2 + b*x + c = 0. The equation is solved using the quadratic formula:
    x = (-b + sqrt(b^2 - 4*a*c)) / (2*a)

For AVX instructions, use:
clang++ -O3 -mavx -fno-tree-vectorize QuadraticEquationStdx_Solution.cpp -o qe.out && ./qe.out

For SSE instructions, use:
clang++ -O3 -msse -fno-tree-vectorize QuadraticEquationStdx_Solution.cpp -o qe.out && ./qe.out
*/

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <chrono>
#include <experimental/simd>
#include <immintrin.h>

// use stdx namespace for experimental SIMD
namespace stdx = std::experimental;
using float_v = stdx::native_simd<float>;

// ANSI escape codes for color output
constexpr const char* COLOR_GREEN = "\033[32m";
constexpr const char* COLOR_RED = "\033[31m";
constexpr const char* COLOR_RESET = "\033[0m";

// SIMD vector size and alignment
constexpr size_t VEC_SIZE = float_v::size();
constexpr size_t VEC_ALIGNMENT = alignof(float_v);

// number of elements
static constexpr size_t N = 8000000;
static_assert(N % VEC_SIZE == 0, "N must be a multiple of SIMD vector size");

static constexpr size_t N_Vectors = N / VEC_SIZE;
static constexpr size_t N_Iterations = 100;        // repeat calculations several times to reduce cache effects

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - STRUCTS - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

struct AOSElement {
  // data
  float a;
  float b;
  float c;
  float x;
};

struct DataAOS {
  // constructor
  DataAOS(int n) {
    data = static_cast<AOSElement*>(_mm_malloc(sizeof(AOSElement) * n, VEC_ALIGNMENT));
  }

  // destructor
  ~DataAOS() {
    if (data) _mm_free(data);
  }

  // data
  AOSElement* data = nullptr;
};

struct DataSOA {
  // constructor
  DataSOA(int n) {
    a = static_cast<float*>(_mm_malloc(sizeof(float) * n, VEC_ALIGNMENT));
    b = static_cast<float*>(_mm_malloc(sizeof(float) * n, VEC_ALIGNMENT));
    c = static_cast<float*>(_mm_malloc(sizeof(float) * n, VEC_ALIGNMENT));
    x = static_cast<float*>(_mm_malloc(sizeof(float) * n, VEC_ALIGNMENT));
  }

  // destructor
  ~DataSOA() {
    if (a) _mm_free(a);
    if (b) _mm_free(b);
    if (c) _mm_free(c);
    if (x) _mm_free(x);
  }

  // data
  float* a = nullptr;
  float* b = nullptr;
  float* c = nullptr;
  float* x = nullptr;
};

struct AOSOAElement {
  // memory assignment
  void SetMemory(float *mem) {
    a = mem;
    b = mem + VEC_SIZE;
    c = mem + 2 * VEC_SIZE;
    x = mem + 3 * VEC_SIZE;
  }

  // data
  float* a = nullptr;
  float* b = nullptr;
  float* c = nullptr;
  float* x = nullptr;
};

struct DataAOSOA {
  // constructor
  DataAOSOA(int n) {
    data = new AOSOAElement[N_Vectors];
    memory = static_cast<float*>(_mm_malloc(sizeof(float) * 4 * N_Vectors * VEC_SIZE, VEC_ALIGNMENT));   // factor 4 for a,b,c,x
    float* mem = memory;

    // assign memory to each vector
    for (int i = 0; i < N_Vectors; ++i) {
      data[i].SetMemory(mem);
      mem += VEC_SIZE * 4;
    }
  }

  // destructor
  ~DataAOSOA() {
    if (memory) _mm_free(memory);
    delete[] data;
  }

  // data and pointer for allocated memory
  float* memory = nullptr;
  AOSOAElement* data = nullptr;
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - CHECKS - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

void CheckResults(const DataAOS& scalar, const DataAOS& simd) {
  bool ok = true;

  for (size_t i = 0; i < N; ++i) {
    if (std::fabs(scalar.data[i].x - simd.data[i].x) > std::fabs(scalar.data[i].x) * 0.01f) {
      ok = false;
      // std::cout << "Element: " << i 
      //           << ", Scalar value: " << scalar.data[i].x 
      //           << ", SIMD value: " << simd.data[i].x 
      //           << ", Difference: " << std::fabs(scalar.data[i].x - simd.data[i].x) 
      //           << std::endl;
    }
  }

  if (!ok)
    std::cout << COLOR_RED 
              << "ERROR! SIMD using AOS and scalar results are not the same." 
              << COLOR_RESET 
              << std::endl;
  else
    std::cout << COLOR_GREEN 
              << "SIMD using AOS and scalar results are the same."
              << COLOR_RESET 
              << std::endl;
}

void CheckResults(const DataAOS& scalar, const DataSOA& simd) {
  bool ok = true;

  for (size_t i = 0; i < N; ++i) {
    if (std::fabs(scalar.data[i].x - simd.x[i]) > std::fabs(scalar.data[i].x) * 0.01f) {
      ok = false;
      // std::cout << "Element: " << i 
      //           << ", Scalar value: " << scalar.data[i].x 
      //           << ", SIMD value: " << simd.x[i] 
      //           << ", Difference: " << std::fabs(scalar.data[i].x - simd.x[i]) 
      //           << std::endl;
    }
  }

  if (!ok)
    std::cout << COLOR_RED 
              << "ERROR! SIMD using SOA and scalar results are not the same."
              << COLOR_RESET 
              << std::endl;
  else
    std::cout << COLOR_GREEN 
              << "SIMD using SOA and scalar results are the same." 
              << COLOR_RESET 
              << std::endl;
}

void CheckResults(const DataAOS& scalar, const DataAOSOA& simd) {
  bool ok = true;

  for (size_t i = 0; i < N; ++i) {
    int nV = i / VEC_SIZE, iV = i % VEC_SIZE;
    if (std::fabs(scalar.data[i].x - simd.data[nV].x[iV]) > std::fabs(scalar.data[i].x) * 0.01f) {
      ok = false;
      // std::cout << "Element: " << i 
      //           << ", Scalar value: " << scalar.data[i].x 
      //           << ", SIMD value: " << simd.data[nV].x[iV] 
      //           << ", Difference: " << std::fabs(scalar.data[i].x - simd.data[nV].x[iV]) 
      //           << std::endl;
    }
  }

  if (!ok) 
    std::cout << COLOR_RED 
              << "ERROR! SIMD using AOSOA and scalar results are not the same."
              << COLOR_RESET 
              << std::endl;
  else
    std::cout << COLOR_GREEN 
              << "SIMD using AOSOA and scalar results are the same." 
              << COLOR_RESET 
              << std::endl;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - MAIN - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

int main() {
  // allocate memory for all variants
  DataAOS dataScalar(N);
  DataAOS dataAOS(N);
  DataSOA dataSOA(N);
  DataAOSOA dataAOSOA(N);

  // initialize parameters with random numbers between 0 and 1
  for (size_t i = 0; i < N; ++i) {
    float a = 0.01f + rand() / static_cast<float>(RAND_MAX);    // avoid division by zero
    float b = rand() / static_cast<float>(RAND_MAX);
    float c = -rand() / static_cast<float>(RAND_MAX);

    dataScalar.data[i] = {a, b, c, 0.f};

    dataAOS.data[i]  = {a, b, c, 0.f};

    dataSOA.a[i] = a;
    dataSOA.b[i] = b;
    dataSOA.c[i] = c;
    
    int nV = i / VEC_SIZE, iV = i % VEC_SIZE;
    dataAOSOA.data[nV].a[iV] = a;
    dataAOSOA.data[nV].b[iV] = b;
    dataAOSOA.data[nV].c[iV] = c;
  }

  // scalar calculations
  std::chrono::high_resolution_clock::time_point scalarTimerStart = std::chrono::high_resolution_clock::now();
  for (int it = 0; it < N_Iterations; ++it) {
    for (size_t i = 0; i < N; ++i) {
      float& a = dataScalar.data[i].a;
      float& b = dataScalar.data[i].b;
      float& c = dataScalar.data[i].c;
      float& x = dataScalar.data[i].x;

      x = (-b + std::sqrt(b * b - 4.f * a * c)) / (2.f * a);
    }
  }
  std::chrono::high_resolution_clock::time_point scalarTimerEnd = std::chrono::high_resolution_clock::now();

  // SIMD with AOS - Part 1
  std::chrono::high_resolution_clock::time_point aosTimerStart = std::chrono::high_resolution_clock::now();
  for (int it = 0; it < N_Iterations; ++it) {
    // TODO
    // Remember: In this data structure, copying data is necessary due to AOS structure in memory
  }
  std::chrono::high_resolution_clock::time_point aosTimerEnd = std::chrono::high_resolution_clock::now();

  // SIMD with SOA - Part 2
  std::chrono::high_resolution_clock::time_point soaTimerStart = std::chrono::high_resolution_clock::now();
  for (int it = 0; it < N_Iterations; ++it) {
    // TODO
    // Remember: In this data structure, copying data is not necessary. 
    // Make use of reinterpret_cast and float_v&.
  }
  std::chrono::high_resolution_clock::time_point soaTimerEnd = std::chrono::high_resolution_clock::now();

  // SIMD with AOSOA - Part 3
  std::chrono::high_resolution_clock::time_point aosoaTimerStart = std::chrono::high_resolution_clock::now();
  for (int it = 0; it < N_Iterations; ++it) {
    // TODO
    // Remember: In this data structure, copying data is not necessary. 
    // Make use of reinterpret_cast and float_v&, but be careful with the memory layout and pointers.
  }
  std::chrono::high_resolution_clock::time_point aosoaTimerEnd = std::chrono::high_resolution_clock::now();

  // report timings and speedups
  double tScalar  = std::chrono::duration<double>(scalarTimerEnd - scalarTimerStart).count() * 1000;
  std::cout << "Time scalar:     " << tScalar << " ms." << std::endl;

  double tAOS = std::chrono::duration<double>(aosTimerEnd - aosTimerStart).count() * 1000;
  std::cout << "Time stdx AOS:   " << tAOS    << " ms, speed up " << (tScalar / tAOS)   << "." << std::endl;

  double tSOA = std::chrono::duration<double>(soaTimerEnd - soaTimerStart).count() * 1000;
  std::cout << "Time stdx SOA:   " << tSOA    << " ms, speed up " << (tScalar / tSOA)   << "." << std::endl;

  double tAOSOA = std::chrono::duration<double>(aosoaTimerEnd - aosoaTimerStart).count() * 1000;
  std::cout << "Time stdx AOSOA: " << tAOSOA  << " ms, speed up " << (tScalar / tAOSOA) << "." << std::endl;

  // compare results
  CheckResults(dataScalar, dataAOS);
  CheckResults(dataScalar, dataSOA);
  CheckResults(dataScalar, dataAOSOA);

  return 0;
}
