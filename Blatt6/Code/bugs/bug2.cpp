/**
  ========================================================================
  Authors: I.Kulakov; M.Zyzak; R. Lakos
  ========================================================================

  To compile and run your code, please use:
  g++ bug2.cpp -o bug2.out -O3 -fopenmp && ./bug2.out
 */

#include <cmath>
#include <iostream>
#include <vector>

#include <omp.h>


constexpr int N_THREADS = 4;

int main() 
{
  int n = 10000;

  std::vector<float> output_scalar(n, 0.0f);
  std::vector<float> output_parallel(n, 0.0f);

  int tmp = 0;
  for (int i = 1; i < n; ++i) {
    tmp += i;
    output_scalar[i] = static_cast<float>(tmp) / i;
  }

  tmp = 0;
  #pragma omp parallel num_threads(N_THREADS)
  {
    #pragma omp for
    for (int i = 1; i < n; ++i) {
        tmp += i;
        output_parallel[i] = static_cast<float>(tmp) / i;
    }
  }

  bool is_correct = true;
  for (int i = 1; i < n; ++i) {
    if (std::fabs(output_scalar[i] - output_parallel[i]) > 1.e-8) {
      is_correct = false;
      break;
    }
  }

  if (is_correct) {
    std::cout << "\033[32m Arrays are the same.\033[0m" << std::endl;
  } else {
    std::cout << "\033[31m ERROR! The output array is not correct!\033[0m" << std::endl;
  }

  return 0;
}
