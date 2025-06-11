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
std::atomic<int> counterParA(0); // TODO: Check required data type for parallelization using atomic
int counterParM; // TODO: Check required data type for parallelization using mutex
spin_mutex mutex; // added global mutex


int ComplicatedFunction( float x ){ // just to simulate some time-consuming calculations, which can be parallelized
  return (int)( cos(sin(x*3.14)) * 10 - 5 );
}


class ApplyTBBA{
  const float  * const a;
  
public:
  void operator()(const blocked_range<long unsigned int> &range, int cpuId = -1) const {
    // TODO parallelization via atomic
    const float  * const a_ptr = a;
    for(int i=range.begin(); i!=range.end(); ++i) {
      if (ComplicatedFunction(a_ptr[i]) == 0) {
        counterParA.fetch_add(1, std::memory_order_relaxed);
      }
    }
  }

  ApplyTBBA(const float * const a_):a(a_){}
  ~ApplyTBBA(){}
};


class ApplyTBBM{
  const float * const a;
  
public:
  void operator()(const blocked_range<long unsigned int> &range, int cpuId = -1) const {
    const float  * const a_ptr = a;    
    for(int i = range.begin(); i != range.end(); ++i){
        // TODO parallelization via mutex
      if (ComplicatedFunction(a_ptr[i]) == 0) {
        spin_mutex::scoped_lock lock(mutex);
        counterParM++;
      }
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
  for(int i = 0; i != N; ++i) {
    if (ComplicatedFunction(a[i]) == 0) counter++;
  }
  timer.Stop();
  
  float timeScalar = timer.RealTime()*1000;
  
  // tbb::global_control c(tbb::global_control::max_allowed_parallelism, 1); // run 1 thread only
  
  // TODO: Implement here your parallelization via atomic counter
  timer.Start();
  parallel_for(blocked_range<long unsigned int>(0,N), ApplyTBBA(a));
  timer.Stop();
  
  float timeParA = timer.RealTime()*1000;
  
  // TODO: Implement here your parallelization via mutex
  timer.Start();
  parallel_for(blocked_range<long unsigned int>(0,N), ApplyTBBM(a));
  timer.Stop();
  
  float timeParM = timer.RealTime()*1000;

  std::cout << "Scalar counter:     " << counter << " \t Time: " << timeScalar << std::endl;
  std::cout << "TBB atomic counter: " << counterParA << " \t Time: " << timeParA << std::endl;
  std::cout << "TBB mutex counter:  " << counterParM << " \t Time: " << timeParM << std::endl;
}  





