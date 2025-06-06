#include <iostream>


int main() {
  int arr[6] = {0, 1, 2, 3, 4, 5};

  int size = sizeof(arr) / sizeof(arr[0]);

  for (int idx = 0; idx < size; idx++) {
    std::cout << "Element: " << arr[idx] << " at " << &arr[idx] << "." << std::endl;
  }

  for (int* ptr = arr; ptr < arr + size; ptr++) {
    std::cout << "Element: " << *ptr << " at " << ptr << " (" << &ptr << ")." << std::endl;
  }

  int* ptr = arr;
  std::cout << sizeof(ptr);
  return 0;
}
