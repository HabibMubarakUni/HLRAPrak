#include<iostream>

void piPointer1(float* pi)
{
  *pi = 3.14;
}

// Diese Funktion gibt einen Zeiger auf dynamisch allokierten Speicher im Heap zurück.
float* piPointer2()
{
  float* pi = new float{3.1415}; // mit new wird Speicher im Heap zugewiesen -> Ausgabe ist ein Pointer
  return pi;
}

int main() {
  float pi{0};  // Initialisierung von pi, da vorher auf nichts gezeigt da Initialisierung fehlte
  float* pi1{&pi}; // Initialisierung des Zeigers pi1 auf die Adresse von pi
  piPointer1(pi1); // Der Wert von pi wird auf 3.14 gesetzt
  std::cout << *pi1 << std::endl; // Ausgabe von pi (3.14)

  float* pi2 = piPointer2(); // Zeiger pi2 auf dynamisch zugewiesenen Speicher
  std::cout << *pi2 << std::endl; // Ausgabe von pi2 (3.1415)
  
  delete pi2; // Speicher im Heap wird freigegeben

  return 0;
}
