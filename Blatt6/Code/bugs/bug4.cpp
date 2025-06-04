/**
  ========================================================================
  Authors: I.Kulakov; M.Zyzak; R. Lakos
  ========================================================================

  To compile and run your code, please use:
  g++ bug4.cpp -o bug4.out -O3 -fopenmp && ./bug4.out
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include <omp.h>


constexpr int N_THREADS = 4;

void CalculateSum(const std::vector<float>& input, int start, int end, float& sum) {
  for (int i = start; i < end; ++i) {
    sum += input[i];
  }
}

int main() 
{
  int n = 10000;

  std::vector<float> input(n), output_scalar(n), output_parallel(n);

  for (int i = 0; i < n; ++i) {
    input[i] = static_cast<float>(rand()) / RAND_MAX;
  }

  float sum = 0.0f;
  CalculateSum(input, 0, n, sum);

  for (int i = 0; i < n; ++i) {
    output_scalar[i] = input[i] / sum;
  }

  sum = 0.0f;

  #pragma omp parallel num_threads(N_THREADS)
  {
    int id = omp_get_thread_num();
    int num_threads = omp_get_num_threads();
    int start = id * n / num_threads;
    int end = (id + 1) * n / num_threads;

    if (id == num_threads - 1) {
      end = n;
    }

    float local_sum = 0.0f;
    CalculateSum(input, start, end, local_sum);

    #pragma omp atomic
    sum += local_sum;

    #pragma omp for
    for (int i = 0; i < n; ++i) {
        output_parallel[i] = input[i] / sum;
    }
  }

  bool is_correct = true;
  for (int i = 0; i < n; ++i) {
    if (std::fabs(output_scalar[i] - output_parallel[i]) > 1.e-8) {
      is_correct = false;
      break;
    }
  }

  if (is_correct) {
    std::cout << "\033[32m Arrays are the same. \033[0m" << std::endl;
  } 
  else {
    std::cout << "\033[31m ERROR! The output array is not correct! \033[0m" << std::endl;
  }

  return 0;
}
