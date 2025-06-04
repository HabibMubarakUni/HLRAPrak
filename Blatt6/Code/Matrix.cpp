/*
Parallelized matrix computations.

To compile and run your source code, please use (other compilers work too):
g++ Matrix.cpp -o Matrix.out -O3 -fno-tree-vectorize -msse -fopenmp -I~/Vc/include -L~/Vc/lib ~/Vc/lib/libVc.a && ./Matrix.out
*/

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <cmath>

#include <omp.h>

#include "utils/TStopWatch.h"
#include "Vc/Vc/Vc"


constexpr int N = 1000;
constexpr int NIter = 100;

alignas(16) float a[N][N];
alignas(16) float c[N][N];
alignas(16) float c_simd[N][N];
alignas(16) float c_omp[N][N];


void checkResults(const float a1[N][N], const float a2[N][N], bool isOMP) 
{
  bool results_correct = true;

  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      if (std::fabs(a1[i][j] - a2[i][j]) > 1.e-8) {
        results_correct = false;
        break;
      }
    }

    if (!results_correct) break;
  }

  if (isOMP) {
    if (results_correct) {
        std::cout << "OpenMP+SIMD and scalar results are the same." << std::endl;
    } else {
        std::cerr << "ERROR! OpenMP+SIMD and scalar results are not the same."  << std::endl;
    }
  }
  else {
    if (results_correct) {
        std::cout << "SIMD and scalar results are the same." << std::endl;
    } else {
        std::cerr << "ERROR! SIMD and scalar results are not the same." << std::endl;
    }
  }
}


int main(int argc, char** argv) 
{
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      a[i][j] = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
    }
  }

  // Scalar computation
  TStopwatch timerScalar;
  for (int ii = 0; ii < NIter; ++ii) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        c[i][j] = std::sqrt(a[i][j]);
      }
    }
  }
  timerScalar.Stop();

  // SIMD computation using Vc
  TStopwatch timerVc;
  for (int ii = 0; ii < NIter; ++ii) {
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; j += Vc::float_v::Size) {
        Vc::float_v& aVec = reinterpret_cast<Vc::float_v&>(a[i][j]);
        Vc::float_v& cVec = reinterpret_cast<Vc::float_v&>(c_simd[i][j]);
        cVec = Vc::sqrt(aVec);
      }
    }
  }
  timerVc.Stop();

  // TODO: OpenMP + SIMD computation
  TStopwatch timerOMP;
  for (int ii = 0; ii < NIter; ++ii) {
    constexpr size_t NUM_THREADS = 4;
    # pragma omp parallel for num_threads(NUM_THREADS)
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j += Vc::float_v::Size) {
        Vc::float_v& aVec = reinterpret_cast<Vc::float_v&>(a[i][j]);
        Vc::float_v& cVec = reinterpret_cast<Vc::float_v&>(c_omp[i][j]);
        cVec = Vc::sqrt(aVec);

      }
    }
    // #pragma omp parallel
    // {
    //   int id = omp_get_thread_num();
    //   int num_threads = omp_get_num_threads();
    //   int start = id * N / num_threads;
    //   int end = (id + 1) * N / num_threads;
  
    //   if (id == num_threads - 1) {
    //     end = N;
    //   }
    //   for (int i = start; i < end; i++) {
    //     for (int j = 0; j < N; j += Vc::float_v::Size) {
    //       Vc::float_v& aVec = reinterpret_cast<Vc::float_v&>(a[i][j]);
    //       Vc::float_v& cVec = reinterpret_cast<Vc::float_v&>(c_omp[i][j]);
    //       cVec = Vc::sqrt(aVec);
    //     }
    //   }
    // }
  }
  timerOMP.Stop();

  double tScalar = timerScalar.RealTime() * 1000;
  double tVc = timerVc.RealTime() * 1000;
  double tOMP = timerOMP.RealTime() * 1000;

  std::cout << std::fixed 
            << std::setprecision(5)
            << "Runtime Scalar:  " << tScalar << " ms\n"
            << "Runtime Vc:      " << tVc << " ms (Speedup: " << tScalar / tVc << ")\n"
            << "Runtime OpenMP:  " << tOMP << " ms (Speedup: " << tScalar / tOMP << ")\n"
            << std::endl;

  checkResults(c, c_simd, false);
  checkResults(c, c_omp, true);

  return EXIT_SUCCESS;
}
