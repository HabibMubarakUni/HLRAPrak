#include <iostream>

#include <stdlib.h>

#include "tbb/parallel_for.h" 
#include "tbb/blocked_range.h"
#include "tbb/global_control.h"

#include "../utils/TStopwatch.h"
#include "../Vc/Vc/Vc"

#include <omp.h>

const int N = 1000; // matrix size. Has to be dividable by 4.

const int NIter = 100; // repeat calculations many times in order to neglect memory read time

float a[N][N] __attribute__ ((aligned(16)));
float c[N][N] __attribute__ ((aligned(16)));
float c_omp[N][N] __attribute__ ((aligned(16)));
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
  float(* const a)[N];
  float(* const c)[N];
  public:

    void operator()(const tbb::blocked_range<long unsigned int> &range, int cpuId = -1) const {
      float(* a_ptr)[N] = a;
      float(* c_ptr)[N] = c;
      for(size_t i=range.begin(); i!=range.end(); ++i){
        for(size_t j=0; j<N; j+=Vc::float_v::Size){
          Vc::float_v &aVec = (reinterpret_cast<Vc::float_v&>(a_ptr[i][j]));
          Vc::float_v &cVec = (reinterpret_cast<Vc::float_v&>(c_ptr[i][j]));
          cVec = f(aVec); 
        }
      }
    }

    ApplyTBB( float a_[N][N],  float c_[N][N]): a(a_), c(c_){}
    ~ApplyTBB(){}
  };


int main() {
  constexpr size_t NUM_THREADS = 4;
  tbb::global_control gcontrol(tbb::global_control::max_allowed_parallelism,NUM_THREADS);

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

  // TODO: Implement a TBB + Vc Version here:
  TStopwatch timerITBB;
  for( int ii = 0; ii < NIter; ii++ )
  {
     // TODO
     tbb::parallel_for(tbb::blocked_range<long unsigned int>(0,N), ApplyTBB(a,c_tbb));
  }
  timerITBB.Stop();
  
  
  // OpenMP+Vc version
  TStopwatch timerOmp;
  for( int ii = 0; ii < NIter; ii++ ){
      #pragma omp parallel for num_threads(NUM_THREADS)
      for( int i = 0; i < N; i++ ) {
          for( int j = 0; j < N; j+=Vc::float_v::Size ) {
              Vc::float_v &aVec = (reinterpret_cast<Vc::float_v&>(a[i][j]));
              Vc::float_v &cVec = (reinterpret_cast<Vc::float_v&>(c_omp[i][j]));
              cVec = f(aVec);
            }
        }
    }
    timerOmp.Stop();
    
    
    
  double tScal = timerScalar.RealTime() * 1000;
  double tOmp = timerOmp.RealTime() * 1000;
  double tITBB = timerITBB.RealTime() * 1000;
  
  std::cout << "Time scalar:     " << tScal << " ms " << std::endl;
  std::cout << "Time OpenMP:     " << tOmp << " ms, speedup factor: " << tScal/tOmp << std::endl;
  std::cout << "Time timerITBB:  " << tITBB << " ms, speedup factor: " << tScal/tITBB << std::endl;
  
  std::cout << "OpenMP: " << std::endl;
  CheckResults(c,c_omp);

  std::cout << "TBB: " << std::endl;
  CheckResults(c,c_tbb);

  return 0;
}
