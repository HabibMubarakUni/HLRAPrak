#include<iostream>


int main() {
  int N = 10;

  int* p1 = new int[N];
  int* p2 = new int[N];

  for (int i = 0; i < N; ++i) {
    p1[i] = i;
    p2[i] = i;
  }

  // p1[11] = 0111;
  p1[15] = 1015;
  // p2[13] = 2013;

  for (int i = 0; i < N; ++i) {
    std::cout << "p1[" << i << "] = " << p1[i] << " " << "p2[" << i << "] = " << p2[i] << std::endl;
  }

  delete[] p1;
  delete[] p2;

  return 0;
}
