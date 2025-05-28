/**
 * @authors: O. Tyagi; R. Lakos; I.Kulakov; M.Zyzak
 * Exercise: Finish the vectorized version using the std::experimental SIMD library.
 * Reference: https://en.cppreference.com/w/cpp/experimental/simd
 *
 * To compile and run this code, you can use <Compiler> Matrix.cpp -O3 -fno-tree-vectorize -msse -o Matrix.out && ./Matrix.out
 */

#include <iostream>
#include <experimental/simd>

#include <stdlib.h>

#include "../fvec/P4_F32vec4.h"
#include "../utils/TStopWatch.h"


namespace stdx = std::experimental;


const int N = 1000;

const int NIter = 100;

float a[N][N] __attribute__((aligned(16)));
float c[N][N] __attribute__((aligned(16)));
float c_simd[N][N] __attribute__((aligned(16)));
float c_simdVc[N][N] __attribute__((aligned(16)));


/*
 * Function template to calculate f(x) = sqrt(x)
 */
template <typename T>
T f(T x) 
{
  return sqrt(x);
}


/*
 * CheckResults function to compare the results of the scalar and SIMD versions
 */
void CheckResults(const float a1[N][N], const float a2[N][N]) 
{
  bool ok = 1;
  for (int i = 0; i < N; i++)
    for (int j = 0; j < N; j++)
      if (fabs(a1[i][j] - a2[i][j]) > 1.e-8) ok = 0;

  if (ok)
    std::cout << "SIMD and scalar results are the same." << std::endl;
  else
    std::cout << "ERROR! SIMD and scalar results are not the same."
              << std::endl;
}


/*
 * Main function including all function calls
 */
int main() 
{
  // fill classes by random numbers
  for (int i = 0; i < N; i++) {
    for (int j = 0; j < N; j++) {
      a[i][j] = float(rand()) / float(RAND_MAX);
    }
  }

  
  // Scalar calculation
  TStopwatch timerScalar;

  for (int ii = 0; ii < NIter; ii++) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j++) {
        c[i][j] = f(a[i][j]);
      }
    }
  }

  timerScalar.Stop();

  // SIMD calculation using fvec
  TStopwatch timerSIMD;

  for (int ii = 0; ii < NIter; ii++) {
    for (int i = 0; i < N; i++) {
      for (int j = 0; j < N; j += fvecLen) {
        fvec &aVec = (reinterpret_cast<fvec &>(a[i][j]));
        fvec &cVec = (reinterpret_cast<fvec &>(c_simd[i][j]));
        cVec = f(aVec);
      }
    }
  } 

  timerSIMD.Stop();

  /// SIMD calculation using stdx
  TStopwatch timerVc;

  const int simdLen = stdx::native_simd<float>::size(); // Die Länge des std::experimental SIMD-Vektors
  std::cout << "simdLen: " << simdLen << "\n" << std::endl;
  //! temp notizen für PDF:
  //! -----------------------------------------------
  //! Ergebnisse:
  //! simdLen: 4
  //!
  //! Time scalar:  86.313 ms 
  //! Time headers: 21.075 ms, speed up 4.09551
  //! Time stdx:      21.059 ms, speed up 4.09862
  //! SIMD and scalar results are the same.
  //! SIMD and scalar results are the same.
  //! -----------------------------------------------
  //! Da simdLen == 4 und N == 1000 durch 4 teilbar => kein Rest
  //! Identisch zur Laufzeit von fvec, da die Längen identisch sind

  for (int ii = 0; ii < NIter; ii++) {
    for (int i = 0; i < N; i++) {
      int j = 0;
      for (; j + simdLen <= N; j += simdLen) { // Bedingung so gestellt, damit keine out-of-bounds-Fehler entstehen
        stdx::native_simd<float>& aVec = reinterpret_cast<stdx::native_simd<float>&>(a[i][j]);
        stdx::native_simd<float>& cVec = reinterpret_cast<stdx::native_simd<float>&>(c_simdVc[i][j]);
        cVec = f(aVec);
      }
      // Restliche Elemente in der Zeile i manuell skalar berechnen
      for (; j < N; j++) {
        c_simdVc[i][j] = f(a[i][j]);
      }
    }
  }
  
  timerVc.Stop();

  double tScal = timerScalar.RealTime() * 1000;
  double tSIMD1 = timerSIMD.RealTime() * 1000;
  double tVc = timerVc.RealTime() * 1000;

  std::cout << "Time scalar:  " << tScal << " ms " << std::endl;
  std::cout << "Time headers: " << tSIMD1 << " ms, speed up " << tScal / tSIMD1 << std::endl;
  std::cout << "Time stdx:      " << tVc << " ms, speed up " << tScal / tVc << std::endl;

  CheckResults(c, c_simd);
  CheckResults(c, c_simdVc);

  return 0;
}
