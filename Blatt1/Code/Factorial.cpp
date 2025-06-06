#include<iostream>


int* GetFactorials(int n){
  
  /* 
  vorher stand: int a[n]; 
  Der Code kompiliert erfolgreich aber damit liegt das Array nur temporär im Stack. 
  Wird automatisch gelöscht, sobald die Funktion endet.
  Ein Zeiger darauf zeigt danach ins Leere.
  */
  int *a = new int[n];
  /*
 
  */

  a[0] = 1;
  for (int i = 1; i < n; ++i)
    a[i] = i*a[i-1];

  return a;
}


int main()
{
  int n = 10;
  int *a = GetFactorials(n);

  for (int i = 0; i < n; ++i) std::cout << a[i] << std::endl;

  delete[] a;
  
  return 0;
}