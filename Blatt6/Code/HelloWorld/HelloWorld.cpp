/**
  Hello, world!.
 */


#include <iostream>
#include <omp.h>

int main() 
{
  // i
  #pragma omp parallel num_threads(10)
  {
    int id = omp_get_thread_num();

    # pragma omp critical // ii
    {
      std::cout << " Hello, world! " << id << std::endl;
    }


  }

  return 0;
}
