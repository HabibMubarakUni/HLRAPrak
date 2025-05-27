// -*- C++ -*-
/*
==================================================
Authors: A. Mithran; I. Kulakov; M. Zyzak
==================================================
*/
/// use "g++ -O3 -fno-tree-vectorize -msse CheckSum.cpp -o 4_3.out && ./4_3.out" to run

// make calculation parallel: a) using SIMD instructions, b) using usual int-instructions.

#include "fvec/P4_F32vec4.h"    // wrapper of the SSE instruction
#include "utils/TStopWatch.h"

#include <iostream>
using namespace std;

#include <stdlib.h> // rand

const int NIter = 100;

const int N = 4000000; // matrix size. Has to be dividable by 4.
unsigned char str[N];

template <typename T>
T Sum(const T* data, const int N)
{
    T sum = 0;

    for (int i = 0; i < N; ++i)
        sum = sum ^ data[i];
    return sum;
}

int main()
{
    // fill string by random values
    for (int i = 0; i < N; i++) {
        str[i] = 256 * (double(rand()) /
                        RAND_MAX); // put a random value, from 0 to 255
    }

    /// -- CALCULATE --

    /// SCALAR

    unsigned char sumS = 0;
    TStopwatch timerScalar;
    for (int ii = 0; ii < NIter; ii++)
        sumS = Sum<unsigned char>(str, N);
    timerScalar.Stop();

    /// SIMD

    unsigned char sumV = 0;

    TStopwatch timerSIMD;
    //! müssen wir noch iwie erklären
    fvec* data_simd = reinterpret_cast<fvec*>(str); 
    fvec simd_sum;
    for (int ii = 0; ii < NIter; ii++) {
        simd_sum = Sum<fvec>(data_simd, N/16);
        unsigned char* simd_to_str = reinterpret_cast<unsigned char*>(&simd_sum);
        sumV = Sum<unsigned char>(simd_to_str, 16);
    }
    timerSIMD.Stop();

    /// SCALAR INTEGER

    unsigned char sumI = 0;

    TStopwatch timerINT;
    //! müssen wir noch iwie erklären
    int* data_int = reinterpret_cast<int*>(str);
    int int_sum;
    for (int ii = 0; ii < NIter; ii++) {
        int_sum = Sum<int>(data_int, N/4);
        sumI = ((int_sum >> 24) ^ (int_sum >> 16) ^ (int_sum >> 8) ^ int_sum);
    }
    timerINT.Stop();

    /// -- OUTPUT --

    double tScal = timerScalar.RealTime() * 1000;
    double tINT = timerINT.RealTime() * 1000;
    double tSIMD = timerSIMD.RealTime() * 1000;

    cout << "Time scalar: " << tScal << " ms " << endl;
    cout << "Time INT:   " << tINT << " ms, speed up " << tScal / tINT << endl;
    cout << "Time SIMD:   " << tSIMD << " ms, speed up " << tScal / tSIMD
         << endl;

    // cout << static_cast<int>(sumS) << " " << static_cast<int>(sumV) << endl;
    if (sumV == sumS && sumI == sumS)
        std::cout << "Results are the same." << std::endl;
    else
        std::cout << "ERROR! Results are not the same." << std::endl;

    return 0;
}