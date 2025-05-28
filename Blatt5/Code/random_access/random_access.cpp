/*
 * @authors: O. Tyagi; R. Lakos; I.Kulakov; M.Zyzak
 * Exercise: Finish the vectorized version using the std::experimental SIMD library.
 *
 *
 * For this exercise, it is important to link against the Vc library. Therefore, run the installation script install_vc.sh first. If you already installed Vc and logged out, you might have to update the paths at the next login. Therefore, please run the export_paths.sh script in that case.
 * To compile and run this code, you can make use of the Makefile by just running "make run", or, use: <Compiler> RandomAccess.cpp -o RandomAccess.out -I~/Vc/include -L~/Vc/lib -O3 -fno-tree-vectorize -msse ~/Vc/lib/libVc.a && ./RandomAccess.out
 */


#include <iostream>
#include <cstdio>
#include <stdlib.h>

#include "Vc/Vc/Vc"
using namespace Vc;


const int N = 100; // Matrix size - Has to be dividable by 4.

float input[N];
float output[N];

int main() {

  unsigned int index[float_v::Size];

  srand( time(NULL) ); // initialize random, so it will start from different numbers
  
  // fill input array with random numbers from 0 to 1
  for(int i = 0; i < N; ++i) {
    input[i] = float(rand())/float(RAND_MAX);
  } 
  
  // fill output array with zeros
  for(int i = 0; i < N; ++i) {
    output[i] = 0.f;
  } 
  
  // fill indices with random numbers from 0 to N-1
  for(int i = 0; i < float_v::Size; ++i) {
    index[i] = static_cast<unsigned int>(float(rand()) / float(RAND_MAX) * N);
  }

  std::cout << "Indices: ";
  for(int i=0; i < float_v::Size; ++i) std::cout << index[i] << " ";
  std::cout << std::endl;
  
  // gather without masking
  float_v tmp;
  //TODO gather data with indices "index" from the array "input" into float_v tmp
  // Use void gather (const float *array, const uint_v &indexes)

  // begin your code here:


  // end of your code
  
  //check results
  bool ok = 1;
  for(int i=0; i < float_v::Size; ++i) {
    if(fabs(tmp[i] - input[index[i]]) > 1.e-7f) ok=0;
  }
  std::cout << "Gather without masking: results are ";
  if(ok) std::cout << "correct." << std::endl;
  else   std::cout << "WRONG." << std::endl;

  // gather with masking
  float_v tmp2;
  //TODO gather data with indices "index" from the array "input" into float_v tmp2, if the value of "input" is larger than 0.5
  // Use void gather (const float *array, const uint_v &indexes, const float_m &mask)

  // begin your code here:


  // end of your code

  //check results
  ok = 1;
  for(int i=0; i<float_v::Size; ++i) {
    if(input[index[i]] > 0.5f) {
      if( fabs(tmp2[i] - input[index[i]]) > 1.e-7f) {
        std::cout << tmp2[i] << " is not equal to " << input[index[i]] << std::endl;
        ok=0;
      }
    }
    else {
      if(fabs(tmp2[i]) > 1.e-7f) {
        std::cout << tmp2[i] << " is not equal to " << 0 << std::endl;
        ok=0;
      }
    }
  }
  
  std::cout << "Gather with masking: results are ";
  if(ok) std::cout << "correct." << std::endl;
  else   std::cout << "WRONG." << std::endl;
  
  //TODO create mask for values for obtained tmp values, which are larger than 0.5 and
  //TODO put all values smaller than 0.5 from tmp to the array "output" at the places given by indices "index"
  // Use void scatter (float *array, const uint_v &indexes, const float_m &mask) const

  // begin your code here:


  // end of your code

  //check results
  ok = true;
  for(int i = 0; i < float_v::Size; ++i) {
    if(tmp[i] < .5f) {
      if(fabs(tmp[i] - output[index[i]]) > 1.e-7f) {
        std::cout << tmp[i] << " is not equal to " << output[index[i]] << std::endl;
        ok = false;
      }
    }
    else {
      if(fabs(output[index[i]]) > 1.e-7f) {
        std::cout << output[index[i]] << " is not equal to " << 0 << std::endl;
        ok = false;
      }
    }
  }
  std::cout << "Scatter with masking: results are ";
  if(ok) std::cout << "correct." << std::endl;
  else   std::cout << "WRONG." << std::endl;
  
  return 0;
}
