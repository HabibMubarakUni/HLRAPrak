/**
 * @authors: O. Tyagi; R. Lakos; I.Kulakov; M.Zyzak
 * Exercise: Finish the vectorized version using the std::experimental SIMD library.
 * Reference: https://en.cppreference.com/w/cpp/experimental/simd
 *
 * To compile and run this code, you can use: <Compiler> Newton.cpp -O3 -fno-tree-vectorize -msse -o Newton.out && ./Newton.out
 */

#include <iostream>
#include <experimental/simd>

#include <stdlib.h>

#include "../fvec/P4_F32vec4.h"
#include "../utils/TStopWatch.h"


namespace stdx = std::experimental;


const int N = 100000;

const float PRECISION = 1e-5;

float par1[N] __attribute__((aligned(16)));
float par2[N] __attribute__((aligned(16)));
float root[N] __attribute__((aligned(16)));


/*
 * Function template to calculate f(x) = p1*x^3 + p2*x^2 + 6*x + 80
 */
template <class T>
T F(const T& x, const T& p1, const T& p2) 
{
  const T x2 = x * x;
  const T x3 = x2 * x;

  return p1 * x3 + p2 * x2 + 6 * x + 80;
}


/*
 * Function template to calculate the derivative: f'(x) = 3*p1*x^2 + 2*p2*x + 6
 */
template <class T>
T Fd(const T& x, const T& p1, const T& p2) {
  const T x2 = x * x;

  return 3 * p1 * x2 + 2 * p2 * x + 6;
}


/*
 * Function to find the root of the function f(x) = p1*x^3 + p2*x^2 + 6*x + 80
 */
float FindRootScalar(const float& p1, const float& p2) {
  float x = 1, x_new = 0;

  for (; abs((x_new - x) / x_new) > PRECISION;) {
    x = x_new;
    x_new = x - F(x, p1, p2) / Fd(x, p1, p2);
  }

  return x_new;
}


/*
 * Function to find the root of the function f(x) = p1*x^3 + p2*x^2 + 6*x + 80 using SIMD
 */
stdx::simd<float> FindRootVectorized(const stdx::simd<float>& p1, const stdx::simd<float>& p2) 
{
  // TODO: Write the vectorized code to find the root using stdx::simd
}


/*
 * Function to check the results
 */
bool CheckResults(float* r) {
  bool ok = true;

  for (int i = 0; i < N; ++i) {
    ok &= fabs(F(root[i], par1[i], par2[i]) / Fd(root[i], par1[i], par2[i])) < PRECISION;
  }

  return ok;
};


/*
 * Function to compare the results
 */
bool CompareResults(float* r1, float* r2) {
  bool ok = true;
  for (int i = 0; i < N; ++i) {
    ok &= fabs(r1[i] - r2[i]) < 1e-7;
  }
  return ok;
};



/*
 * Main function including all function calls
 */
int main() {
  // Fill parameter arrays with random numbers
  for (int i = 0; i < N; ++i) {
    par1[i] = 1 + double(rand()) / double(RAND_MAX);
    par2[i] = double(rand()) / double(RAND_MAX);
  }

  // Scalar calculation
  TStopwatch timer;

  for (int i = 0; i < N; ++i) {
    root[i] = FindRootScalar(par1[i], par2[i]);
  }

  timer.Stop();

  std::cout << "Scalar part:" << std::endl;

  if (CheckResults(root))
    std::cout << "Results are correct!" << std::endl;
  else
    std::cout << "Results are INCORRECT!" << std::endl;

  std::cout << "Time: " << timer.RealTime() * 1000 << " ms " << std::endl;
  
  float timeScalar = timer.RealTime();

  // SIMD calculation using stdx::simd

  const int Nv = N / stdx::simd<float>::size();
  stdx::simd<float> par1_v[Nv];
  stdx::simd<float> par2_v[Nv];
  stdx::simd<float> root_v[Nv];
  float root2[N];

  // Copy input using copy_from
  // TODO copy the data to par1_v and par2_v 

  // Compute the roots
  timer.Start();
  for (int i = 0; i < Nv; ++i) {
    root_v[i] = FindRootVectorized(par1_v[i], par2_v[i]);
  }
  timer.Stop();

  // Copy output using copy_to
  // TODO copy the data back to root2


  std::cout << "SIMD part:" << std::endl;

  if (CompareResults(root, root2))
    std::cout << "Results are the same!" << std::endl;
  else
    std::cout << "Results are NOT the same!" << std::endl;

  std::cout << "Time: " << timer.RealTime() * 1000 << " ms " << std::endl;
  
  float timeVectorized = timer.RealTime();

  std::cout << "Speed up: " << timeScalar / timeVectorized << std::endl;

  return 0;
}
