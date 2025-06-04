/**
  ========================================================================
  Authors: I.Kulakov; M.Zyzak; R. Lakos
  ========================================================================

  To compile and run your code, please use:
  g++ bug3.cpp -o bug3.out -O3 -fopenmp && ./bug3.out
 */

#include <cstdlib>
#include <cmath>
#include <iostream>
#include <vector>

#include <omp.h>


constexpr int N_THREADS = 4;

int main() 
{
  int n = 1000;

  std::vector<float> input(n), output_scalar(n), output_parallel(n);

  for (int i = 0; i < n; ++i) {
    input[i] = static_cast<float>(rand()) / RAND_MAX; // jedem input[i] wurde eine zufällige Zahl zugewiesen
  }

  float sum = 0.0f;

  for (int i = 0; i < n; ++i) {
    sum += input[i]; // Summe der zufälligen Zahl berechnet
  }
  for (int i = 0; i < n; ++i) {
    output_scalar[i] = input[i] / sum; // Anteil berechnet
  }

  sum = 0.0f;

  #pragma omp parallel firstprivate(n) num_threads(N_THREADS) // jeder Thread hat den Wert n=1000, wegen firstprivate
  {
    #pragma omp for // nowait -> Kein Warten nach for-loop! Andere Threads können hier sofort weitermachen
    for (int i = 0; i < n; ++i) {
      #pragma omp atomic
      sum += input[i]; // nur diese Zeile wird nicht parallelisiert
    }

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
    std::cout << "\033[32m Arrays are the same.\033[0m" << std::endl;
  } 
  else {
    std::cout << "\033[31m ERROR! The output array is not correct!\033[0m" << std::endl;
  }

  return 0;
}
