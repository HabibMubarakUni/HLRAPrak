#include <iostream>
#include <atomic>
#include <cmath>

#include <stdlib.h>
#include <stdio.h>

#include "tbb/parallel_for.h"
#include "tbb/blocked_range.h"
#include <tbb/global_control.h>
#include "tbb/spin_mutex.h"
using namespace tbb;

#include "../utils/TStopwatch.h"

const int N = 2000000;

int counter = 0;
int counterParA; // TODO: Check required data type for parallelization using atomic
int counterParM; // TODO: Check required data type for parallelization using mutex


int ComplicatedFunction( float x ){ // just to simulate some time-consuming calculations, which can be parallelized
  return (int)( cos(sin(x*3.14)) * 10 - 5 );
}


class ApplyTBBA{
  const float  * const a;
  
public:
  void operator()(const blocked_range<long unsigned int> &range, int cpuId = -1) const {
    // TODO parallelization via atomic
  }

  ApplyTBBA(const float * const a_):a(a_){}
  ~ApplyTBBA(){}
};


class ApplyTBBM{
  const float * const a;
  
public:
  void operator()(const blocked_range<long unsigned int> &range, int cpuId = -1) const {
    for(int i = range.begin(); i != range.end(); ++i){
        // TODO parallelization via mutex
    }
  }

  ApplyTBBM(const float * const a_):a(a_){}
  ~ApplyTBBM(){}
};


int main ()
{
  float a[N];
  
  // fill classes by random numbers
  for( int i = 0; i < N; i++ ) {
    a[i] = (float(rand())/float(RAND_MAX));
  }
  
  TStopwatch timer;
  for(int i = 0; i != N; ++i){
    if (ComplicatedFunction(a[i]) == 0) counter++;
  }
  timer.Stop();
  
  float timeScalar = timer.RealTime()*1000;
  
  // tbb::global_control c(tbb::global_control::max_allowed_parallelism, 1); // run 1 thread only
  
  // TODO: Implement here your parallelization via atomic counter
  timer.Start();

  timer.Stop();
  
  float timeParA = timer.RealTime()*1000;
  
  // TODO: Implement here your parallelization via mutex
  timer.Start();

  timer.Stop();
  
  float timeParM = timer.RealTime()*1000;

  std::cout << "Scalar counter:     " << counter << " \t Time: " << timeScalar << std::endl;
  std::cout << "TBB atomic counter: " << counterParA << " \t Time: " << timeParA << std::endl;
  std::cout << "TBB mutex counter:  " << counterParM << " \t Time: " << timeParM << std::endl;
}  





