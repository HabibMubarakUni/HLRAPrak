#include <iostream>

/*
Die Funktion funktioniert für jeden Datentype T, z.B. int, double, float, short, char
*/
template <typename T>
T GetMax (T a, T b) {
  T result;

  if (a > b) result = a;
  else result = b;

  return result;
}


int main () {
  int i=5, j=6;
  double l=9.2, m=2e+9;

  /*
  Wir gehen davon aus, dass die Ausgabe 6 sein wird,
  weil 6 > 5 ist und kein Casting stattfindet
  */
  std::cout << "int: " << GetMax<int>(i, j) << std::endl;
  // AUSGABE: 6  ==> Erwartung erfüllt

  /*
  Wir erwarten, dass die Ausgabe 2e+09 ist, 
  weil 2e+09 > 9,2 ist und kein Casting stattfindet
  */
  std::cout << "double: " << GetMax<double>(l, m) << std::endl;
  // AUSGABE: 2e+09  ==> Erwartung erfüllt

  /*
  Wir erwarten, dass die Ausgabe 2e+09 sein wird, 
  weil 2e+09 > 9,2 ist und float eine Größe von 32 Bits hat, was ausreicht, 
  um 2e+09 im IEEE 754-Standard darzustellen.
  */
  std::cout << "float: " <<  GetMax<float>(l, m) << std::endl;
  // AUSGABE: 2e+09  ==> Erwartung erfüllt

  /*
  Wir erwarten, dass die Ausgabe 2e+09 sein wird, 
  weil 2e+09 > 9 ist und int hat auch eine Größe von 32 Bits, 
  was ausreicht, um 2e+09 darzustellen, und 9,2 wird in 9 umgewandelt
  */
  std::cout << "int: " <<  GetMax<int>(l, m) << std::endl;
  // AUSGABE: 2000000000  ==> Erwartung im Grunde erfüllt

  /*
  Wir wissen nicht, wie die Ausgabe aussehen wird, 
  weil wir nicht genau weiß, wie der Overflow gehandhabt werden soll
  */
  std::cout << "short: " <<  GetMax<short>(l, m) << std::endl;
  // AUSGABE: 9 
  /*
  Die Ausgabe ist 9, weil short eine Größe von 16 Bits hat, 
  die nicht ausreicht um 2e+09 darzustellen und nur die ersten 16 Bits 
  von 2e+09 an die neue Speicherstelle übertragen werden, 
  die in diesem Fall -27648 (im Zweierkomplement) und 9 > -27648 darstellen
  */

  /*
  Wir wissen nicht, wie die Ausgabe aussehen wird, 
  weil wir nicht genau weiß, wie der Overflow gehandhabt werden soll
  */
  std::cout << "char: " <<  GetMax<char>(l, m) << std::endl;
  // AUSGABE:  <== HT (horizontal tab) 
  /*
  Die Ausgabe ist   (9), weil char eine Größe von 8 Bits hat, was nicht ausreicht,
  um 2e+09 darzustellen und nur die ersten 8 Bits von 2e+09 an die neue Speicherstelle übertragen werden,
  die in diesem Fall 0 darstellen, so dass 9 > 0 und 9 der ASCII-Code für HT (horizontaler Tabulator) ist
  */

  return 0;
}
