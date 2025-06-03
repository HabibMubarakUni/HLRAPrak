/**
 * C++ implementation of the Newton's method to find the root of a polynomial function
 * http://en.wikipedia.org/wiki/Newton's_method
 * @authors: R. Lakos; A. Mithran; O. Tyagi;
 * Reference : https://en.cppreference.com/w/cpp/experimental/simd
 *
 * Please use "make run-sse" or "make run-avx" to compile and run this code using SSE or AVX instructions respectively.
 */

#include <experimental/simd>
namespace stdx = std::experimental;

#include <chrono>
#include <iostream>

// type aliases for chrono types for shorter usage
using steady_clock = std::chrono::steady_clock;
using time_point = std::chrono::time_point<std::chrono::steady_clock>;
using duration = std::chrono::duration<double>;

// constants and aligned data arrays
const int N = 100000;
float par1[N], par2[N] __attribute__((aligned(16)));
float root[N] __attribute__((aligned(16)));
const float P = 1e-5f;

// ========================================================= Helper functions for SIMD operations =========================================================

/**
 * Function to find the maximum of a SIMD vector and a scalar value.
 */
template <typename T>
stdx::simd<T> simd_max(const stdx::simd<T>& a, const T& b) {
  stdx::simd<T> result = stdx::simd<T>(b);
  stdx::where(a > b, result) = a;
  return result;
}

/**
 * Function to find the maximum of two SIMD vectors.
 */
template <typename T>
stdx::simd<T> simd_max(const stdx::simd<T>& a, const stdx::simd<T>& b) {
  stdx::simd<T> result = b;
  stdx::where(a > b, result) = a;
  return result;
}

// ========================================================= Polynomial and its derivative =========================================================

/**
 * Polynomial function F(x) = p1 * x^3 + p2 * x^2 + 6 * x + 80
 * where p1 and p2 are parameters.
 */
template <class T>
T F(const T& x, const T& p1, const T& p2) {
  T x2 = x * x;
  return p1 * x2 * x + p2 * x2 + 6 * x + 80;
}

/**
 * Derivative of the polynomial F(x) = p1 * x^3 + p2 * x^2 + 6 * x + 80 -> Fd(x) = 3 * p1 * x^2 + 2 * p2 * x + 6
 * where p1 and p2 are parameters.
 */
template <class T>
T Fd(const T& x, const T& p1, const T& p2) {
  return 3 * p1 * x * x + 2 * p2 * x + 6;
}


// ========================================================= Root finding functions =========================================================

/**
 * Function to find the root of the polynomial F using Newton's method.
 */
float FindRootScalar(const float& p1, const float& p2) {
  float x = 1.f;
  float x_new = x - F(x, p1, p2) / Fd(x, p1, p2);

  while (std::abs(x_new - x) > P * std::max(std::abs(x), 1.f)) {
    x = x_new;
    x_new = x - F(x, p1, p2) / Fd(x, p1, p2);
  }
  return x_new;
}

/**
 * Function to find the root of the polynomial F using SIMD version of Newton's method.
 */

stdx::simd<float> FindRootVector(const stdx::simd<float>& p1, const stdx::simd<float>& p2) {
  stdx::simd<float> x = 1.f;
  stdx::simd<float> x_new = x - F(x, p1, p2) / Fd(x, p1, p2);

  // // mit n = 1000 Iterationen:
  // const size_t n_iter = 1000;
  // for (size_t i = 0; i < n_iter; i++) {
  //   x = x_new;
  //   x_new = x - F(x, p1, p2) / Fd(x, p1, p2);
  // }

  // mit Masken:
  stdx::simd_mask<float> mask = stdx::abs(x_new - x) > P * simd_max(stdx::abs(x), stdx::simd<float>(1.f));
  while (stdx::any_of(mask)) {
    stdx::where(mask, x) = x_new; // Diese Zeile musste geändert werden
    stdx::where(mask, x_new) = x - F(x, p1, p2) / Fd(x, p1, p2);
    mask = stdx::abs(x_new - x) > P * simd_max(stdx::abs(x), stdx::simd<float>(1.f));
  }


  return x_new;
}

// ========================================================= Result checking functions =========================================================

/**
 * Function to check the results of the scalar root finding.
 */
bool CheckResults(float* r) {
  bool ok = true;
  for (int i = 0; i < N; ++i) {
    ok &= std::fabs(F(root[i], par1[i], par2[i]) / Fd(root[i], par1[i], par2[i])) < P;
  }
  return ok;
}

/**
 * Function to compare the results of scalar and SIMD root finding.
 */
bool CompareResults(float* r1, float* r2) {
  bool ok = true;
  for (int i = 0; i < N; ++i) {
    ok &= std::fabs(r1[i] - r2[i]) < 1e-7;
  }
  return ok;
}

// ========================================================= Main function =========================================================

int main() {
  // set random parameter values
  for (int i = 0; i < N; ++i) {
    par1[i] = 1 + double(rand()) / double(RAND_MAX);  // from 1 to 2
    par2[i] = double(rand()) / double(RAND_MAX);      // from 0 to 1
  }

  // scalar part
  time_point startS = steady_clock::now();
  for (int i = 0; i < N; ++i) {
    root[i] = FindRootScalar(par1[i], par2[i]);
  }
  time_point endS = steady_clock::now();
  duration elapsedS = endS - startS;

  std::cout << "Scalar part:" << std::endl;
  if (CheckResults(root))
    std::cout << "Results are correct!" << std::endl;
  else
    std::cout << "Results are INCORRECT!" << std::endl;
  std::cout << "Time: " << elapsedS.count() * 1000 << " ms" << std::endl;
  float timeS = static_cast<float>(elapsedS.count());

  /// SIMD part

  const int Nv = N / stdx::simd<float>::size();
  stdx::simd<float> par1_v[Nv], par2_v[Nv];
  stdx::simd<float> root_v[Nv];
  float root2[N];

  // copy input to SIMD vectors
  for (int i = 0; i < Nv; ++i) {
      par1_v[i].copy_from(&par1[i * stdx::simd<float>::size()], stdx::element_aligned);
      par2_v[i].copy_from(&par2[i * stdx::simd<float>::size()], stdx::element_aligned);
  }

  // compute with SIMD
  time_point startV = steady_clock::now();
  for (int i = 0; i < Nv; ++i) {
    root_v[i] = FindRootVector(par1_v[i], par2_v[i]);
  }
  time_point endV = steady_clock::now();
  duration elapsedV = endV - startV;

  // copy output from SIMD vectors
  for (int i = 0; i < Nv; ++i) {
      root_v[i].copy_to(&root2[i * stdx::simd<float>::size()], stdx::element_aligned);
  }

  std::cout << "SIMD part:" << std::endl;
  if (CompareResults(root, root2))
    std::cout << "Results are the same!" << std::endl;
  else
    std::cout << "Results are NOT the same!" << std::endl;
  std::cout << "Time: " << elapsedV.count() * 1000 << " ms" << std::endl;
  float timeV = static_cast<float>(elapsedV.count());

  std::cout << "Speed up: " << timeS / timeV << std::endl;

  // Der Code ab hier ist neu hinzugefügt
  float max_difference = 0;
  float scalar_root;
  float simd_root;
  for (int i = 0; i < N; i++) {
    float difference = std::abs(root[i] - root2[i]);
    if (difference > max_difference) {
      max_difference = difference;
      scalar_root = root[i];
      simd_root = root2[i];
    }
  }
  std::cout << "\nThe max difference is: " << max_difference << std::endl;


  return 0;
}
