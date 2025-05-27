// -*- C++ -*-
/*
==================================================
Authors: A. Mithran; I. Kulakov; M. Zyzak
==================================================
*/
/// use "g++ -O3 -fno-tree-vectorize -msse QuadraticEqn.cpp -o 4_2_1.out && ./4_2_1.out" to run
/// Note:
/// __m128 - SIMD vector
/// SIMD intrinsics:
/// _mm_set_ps(f3,f2,f1,f0) - write 4 floats into the SIMD vector
/// note, that the order of entries is inversed
/// _mm_set_ps1(a) - write float "a" into the SIMD vector
/// _mm_add_ps(a,b) - a+b
/// _mm_sub_ps(a,b) - a-b
/// _mm_mul_ps(a,b) - a*b
/// _mm_div_ps(a,b) - a/b
/// _mm_sqrt_ps(a) - sqrt(a)
/// type2* pointer2 = reinterpret_cast<type2*>( pointer1 ) - change pointer type

#include "fvec/P4_F32vec4.h"    // wrapper of the SSE instruction
#include "utils/TStopWatch.h"

#include <cmath>
#include <iostream>

#include <stdlib.h> // rand

static const int NVectors = 10000;
static const int N = NVectors * fvecLen;

static const int NIterOut = 1000;

void CheckResults(const float* yScalar, const float* ySIMD, const int NSIMD)
{
    bool ok = true;
    for (int i = 0; i < N; i++)
        if (fabs(yScalar[i] - ySIMD[i]) > yScalar[i] * 0.01) {
            ok = false;
            //      std::cout << i<<" " << yScalar[i] << " " << ySIMD[i] << " " << fabs(yScalar[i] - ySIMD[i])<<std::endl;
        }
    if (!ok)
        std::cout << "ERROR! SIMD" << NSIMD
                  << " and scalar results are not the same." << std::endl;
    else
      std::cout << "SIMD" << NSIMD << " and scalar results are the same." << std::endl;
}

int main()
{
    //input data
    float* a = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* b = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* c = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    //output data
    float* x = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd1 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd2 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd3 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));
    float* x_simd4 = (float*) _mm_malloc(sizeof(float)*N,16);// __attribute__ ((aligned(16)));

    // fill parameters by random numbers
    for (int i = 0; i < N; i++) {
        a[i] = 0.01 + float(rand()) / float(RAND_MAX); // put a random value, from 0.01 to 1.01 (a has not to be equal 0)
        b[i] = float(rand()) / float(RAND_MAX);
        c[i] = -float(rand()) / float(RAND_MAX);
    }

    /// -- CALCULATE --

    // scalar calculations
    TStopwatch timerScalar;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < N; i++) {
            // Nullstellen mit abc-Formel
            float det = b[i] * b[i] - 4 * a[i] * c[i];
            x[i] = (-b[i] + sqrt(det)) / (2 * a[i]);
        }
    timerScalar.Stop();

    // SIMD calculations with SIMD intrinsics and data copy
    TStopwatch timerSIMD;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < NVectors; i++) {
            ///__put your code here__
            /// copy coefficients b and c
            __m128 aV = _mm_set_ps(a[i * fvecLen+3], a[i * fvecLen + 2],
                                   a[i * fvecLen + 1], a[i * fvecLen]);
            __m128 bV = _mm_set_ps(b[i * fvecLen+3], b[i * fvecLen + 2],
                                   b[i * fvecLen + 1], b[i * fvecLen]);
            __m128 cV = _mm_set_ps(c[i * fvecLen+3], c[i * fvecLen + 2],
                                   c[i * fvecLen + 1], c[i * fvecLen]);
            
            ///__put your code here__

            // Berechnung von: b^2 - 4ac
            __m128 detV = _mm_sub_ps(
                _mm_mul_ps(bV, bV),  // b^2
                _mm_mul_ps( // 4*(a*c)
                    _mm_set_ps1(4), 
                    _mm_mul_ps(aV, cV)
                )
            );
            // Berechnung von: (sqrt(det) - b) / (2*a)
            __m128 xV = _mm_div_ps(
                _mm_sub_ps( // sqrt(det) - b
                    _mm_sqrt_ps(detV), // sqrt(det)
                    bV),
                    _mm_mul_ps(_mm_set_ps1(2), aV)); // 2*a


            // kopiere output Daten
            // Diese Schleife extrahiert die 4 berechneten Werte aus dem SIMD-Register xV 
            // und speichert sie im Array x_simd1 an der richtigen Position
            for(int iE=0; iE<fvecLen; iE++)
                x_simd1[i*fvecLen+iE] = (reinterpret_cast<float*>(&xV))[iE];
        }
    timerSIMD.Stop();

    // SIMD calculations with SIMD intrinsics and reinterpret_cast
    TStopwatch timerSIMD2;
    for(int io=0; io<NIterOut; io++)
        for(int i=0; i<N; i+=fvecLen) {

            /// cast coefficients b and c (and a for detV and xV)
            // 4 Werte werden eingelesen wegen __m128
            __m128& aV = (reinterpret_cast<__m128&>(a[i]));
            __m128& bV = (reinterpret_cast<__m128&>(b[i]));
            __m128& cV = (reinterpret_cast<__m128&>(c[i]));

            // Berechnung von: b^2 - 4ac
            __m128 detV = _mm_sub_ps(
                _mm_mul_ps(bV, bV),  // b^2
                _mm_mul_ps(
                    _mm_set_ps1(4.0f),  // 4
                    _mm_mul_ps(aV, cV)) // a*c
                );
            
            // Berechnung von:  (-b + sqrt(det)) / (2*a)
            __m128 xV = _mm_div_ps(
                _mm_sub_ps(
                    _mm_sqrt_ps(detV),bV), // (sqrt(det) - b)
                    _mm_mul_ps(_mm_set_ps1(2.0f), aV) // 2*a
                    );
            reinterpret_cast<__m128&>(x_simd2[i]) = xV;
        }
    timerSIMD2.Stop();

    // SIMD clculations with headers and data copy
    TStopwatch timerSIMD3;
    for (int io = 0; io < NIterOut; io++) {
        for (int i = 0; i < NVectors; i++) {
            // copy input data
            // copy coefficients b and c
            fvec aV = fvec(a[i*fvecLen], a[i*fvecLen+1], a[i*fvecLen+2], a[i*fvecLen+3]);
            fvec bV = fvec(b[i*fvecLen], b[i*fvecLen+1], b[i*fvecLen+2], b[i*fvecLen+3]);
            fvec cV = fvec(c[i*fvecLen], c[i*fvecLen+1], c[i*fvecLen+2], c[i*fvecLen+3]);

            // calculate the root
            fvec detV = bV * bV - 4 * aV * cV;
            fvec xV = (sqrt(detV) - bV) / (2 * aV);

            // copy output data
            for(int iE=0; iE<fvecLen; iE++)
                x_simd3[i*fvecLen+iE] = xV[iE];
        }
    }
    timerSIMD3.Stop();

    // SIMD clculations with headers and reinterpret_cast
    TStopwatch timerSIMD4;
    for (int io = 0; io < NIterOut; io++)
        for (int i = 0; i < N; i += fvecLen) {
            // TODO__put your code here__
            // TODO cast coefficients b and c
            fvec& aV = reinterpret_cast<fvec&>(a[i]);
            fvec& bV = reinterpret_cast<fvec&>(b[i]);
            fvec& cV = reinterpret_cast<fvec&>(c[i]);

            // TODO__put your code here__
            // TODO put the code, which calculates the root
            fvec detV = bV * bV - 4 * aV * cV;
            fvec xV = (-bV + sqrt(detV)) / (2 * aV);

            reinterpret_cast<fvec&>(x_simd4[i]) = xV;
        }
    timerSIMD4.Stop();

    double tScal = timerScalar.RealTime() * 1000;
    double tSIMD1 = timerSIMD.RealTime() * 1000;
    double tSIMD2 = timerSIMD2.RealTime() * 1000;
    double tSIMD3 = timerSIMD3.RealTime() * 1000;
    double tSIMD4 = timerSIMD4.RealTime() * 1000;
    std::cout << "Time scalar: " << tScal << " ms " << std::endl;
    std::cout << "Time SIMD1:   " << tSIMD1 << " ms, speed up "
              << tScal / tSIMD1 << std::endl;    
    std::cout << "Time SIMD2:   " << tSIMD2 << " ms, speed up "
              << tScal / tSIMD2 << std::endl;
    std::cout << "Time SIMD3:   " << tSIMD3 << " ms, speed up "
              << tScal / tSIMD3 << std::endl;
    std::cout << "Time SIMD4:   " << tSIMD4 << " ms, speed up "
              << tScal / tSIMD4 << std::endl;
    
    //compare SIMD and scalar results
    CheckResults(x, x_simd1, 1);
    CheckResults(x, x_simd2, 2);
    CheckResults(x, x_simd3, 3);
    CheckResults(x, x_simd4, 4);
    return 1;
}
