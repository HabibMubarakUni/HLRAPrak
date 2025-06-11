#include <iostream>

#include <stdlib.h>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"

#include "../utils/TStopwatch.h"
#include "../Vc/Vc/Vc"


const int N = 1000; // matrix size. Has to be dividable by 4.

const int NIter = 100; // repeat calculations many times in order to neglect memory read time

float a[N][N] __attribute__ ((aligned(16)));
float c[N][N] __attribute__ ((aligned(16)));
float c_simd[N][N] __attribute__ ((aligned(16)));
float c_tbb[N][N] __attribute__ ((aligned(16)));


template<typename T>
T f(T x) {
  return sqrt(x);
}


void CheckResults(const float a1[N][N], const float a2[N][N])
{
  bool ok = 1;
  for(int i=0; i<N; i++)
    for(int j=0; j<N; j++)
      if( fabs(a1[i][j] - a2[i][j]) > 1.e-8 ) ok = 0;

  if(ok)
    std::cout << "Parallel and scalar results are the same." << std::endl;
  else
    std::cout << "ERROR! Parallel and scalar results are not the same." << std::endl;
}

class ApplyTBB{ // TODO: finish
public:

  void operator()(const tbb::blocked_range<long unsigned int> &range, int cpuId = -1) const {
  }

//   ApplyTBB( float a_[N][N],  float c_[N][N]): a(a_), c(c_){}
  ~ApplyTBB(){}
};


int main() {

  // fill classes by random numbers
  for( int i = 0; i < N; i++ ) {
    for( int j = 0; j < N; j++ ) {
      a[i][j] = float(rand())/float(RAND_MAX); // put a random value, from 0 to 1
    }
  } 
  
  // Scalar version
  TStopwatch timerScalar;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j++ ) {
        c[i][j] = f(a[i][j]);
      }
    }
  timerScalar.Stop();
  
  // Vc version
  TStopwatch timerVc;
  for( int ii = 0; ii < NIter; ii++ )
    for( int i = 0; i < N; i++ ) {
      for( int j = 0; j < N; j+=Vc::float_v::Size ) {
          Vc::float_v &aVec = (reinterpret_cast<Vc::float_v&>(a[i][j]));
          Vc::float_v &cVec = (reinterpret_cast<Vc::float_v&>(c_simd[i][j]));
          cVec = f(aVec);
      }
    }
  timerVc.Stop();
  
  // TODO: Implement a TBB + Vc Version here:
  TStopwatch timerITBB;
  for( int ii = 0; ii < NIter; ii++ )
  {
     // TODO
  }
  timerITBB.Stop();
  
  double tScal = timerScalar.RealTime() * 1000;
  double tVc = timerVc.RealTime() * 1000;
  double tITBB = timerITBB.RealTime() * 1000;
  
  std::cout << "Time scalar:     " << tScal << " ms " << std::endl;
  std::cout << "Time Vc:         " << tVc << " ms, speedup factor: " << tScal/tVc << std::endl;
  std::cout << "Time timerITBB:  " << tITBB << " ms, speedup factor: " << tScal/tITBB << std::endl;
  
  std::cout << "Vc: " << std::endl;
  CheckResults(c,c_simd);

  std::cout << "TBB: " << std::endl;
  CheckResults(c,c_tbb);

  return 0;
}
