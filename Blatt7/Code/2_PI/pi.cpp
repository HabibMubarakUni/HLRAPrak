/*
This program will numerically compute the integral of

                  4/(1+x*x) 
  
from 0 to 1.  The value of this integral is pi.
*/

#include <iostream>
  
#include <stdio.h>

#include "tbb/global_control.h"

#include "../utils/TStopwatch.h"

static long num_steps = 2000000000;
double step;

int main ()
{
  int i;
  double x, pi, sum = 0.0;
  double start_time, run_time;

  step = 1.0/(double) num_steps;
  
  // tbb::global_control c(tbb::global_control::max_allowed_parallelism, 1); // run 1 thread only
  
  TStopwatch timer;

  // TODO parallelize the loop
  for(int i = 1; i != num_steps; ++i){
    const double x = (i-0.5)*step;
    sum += 4.0/(1.0+x*x);
  }

  pi = step * sum;
  
  timer.Stop();
  run_time = timer.RealTime() * 1000;

  std::cout << "Pi: " << pi << std::endl;
  std::cout << "Time: " << run_time << " ms"<< std::endl;
  std::cout << "Steps: " << num_steps << std::endl;
}  





