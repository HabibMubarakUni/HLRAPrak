/**
  ========================================================================
  Authors: I.Kulakov; M.Zyzak; R. Lakos
  ========================================================================

  To compile and run your code, please use:
  g++ bug1.cpp -o bug1.out -O3 -fopenmp && ./bug1.out
 */


#include <cstdlib>
#include <iostream>
#include <vector>

#include <omp.h>


constexpr int N_THREADS = 4;

int main() 
{
  int n = 1000;

  std::vector<float> input(n), output(n);

  for (int i = 0; i < n; ++i) {
    input[i] = static_cast<float>(rand()) / RAND_MAX;
  }

  #pragma omp parallel private(n) num_threads(N_THREADS)
  {
    #pragma omp for
    for (int i = 0; i < n; ++i) {
      output[i] = input[i];
    }
  }

  bool is_correct = true;
  for (int i = 0; i < n; ++i) {
    if (output[i] != input[i]) {
      is_correct = false;
      break;
    }
  }

  if (is_correct) {
    std::cout << "\033[32m Arrays are the same.\033[0m" << std::endl;
  } 
  else {
    std::cout << "\033[31m ERROR! The output array is not correct!\033[0m" << std::endl;
  }

  return 0;
}
