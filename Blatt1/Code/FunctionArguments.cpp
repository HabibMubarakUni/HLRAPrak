#include<iostream>

/* 
Wenn man int arg als Parameter ohne & oder * übergibst, arbeitet man mit einer Kopie.
Änderungen in der Funktion betreffen nur diese Kopie
Das Original bleibt unverändert es sei denn man gibt den neuen Wert zurück und speichert ihn
oder arbeitest mit Zeiger oder Referenz
*/
void number_increment(int arg)
{
  ++arg;
}

/*
arg ist wieder eine Kopie
Diese Kopie wird lokal verändert (++arg) und zurückgegeben
*/
int copy_increment(int arg)
{
  ++arg;

  return arg;
}
  
/*
Man übergibt &number (die Adresse der Variable).
arg ist also ein Zeiger (auf number)
*arg bedeutet: Man geht an die Adresse und greift auf den Wert zu.
++*arg erhöht also direkt den Wert von number.
*/
void pointer_increment(int* arg)
{
  // TODO
  ++*arg;
}

/*
arg ist eine Referenz auf number – kein Zeiger, sondern ein anderen Namen
*/
void reference_increment(int& arg)
{
  // TODO
  ++arg;
}


int main () {
  int number = 0;

  std::cout << "Number is: " << number  << std::endl;

  // 1st case:
  number_increment(number);
  // Der Funktionsaufruf number_increment(number) erzeugt eine Kopie von number
  // Es wird ein neuer Speicherplatz für arg angelegt.
  // arg = 0 (Kopie des Werts von number)
  // ++arg erhöht diese Kopie → arg = 1
  // Funktion endet → arg wird gelöscht (Speicher freigegeben)
  // number bleibt unverändert bei 0
  std::cout << "Number is: " << number  << std::endl;

  // 2nd case:
  number = copy_increment(number);
  // Man überschreibt number mit dem Rückgabewert von arg
  std::cout << "Number is: " << number << std::endl;

  // 3rd case:
  pointer_increment( &number /* TODO */ );
  // geht an die Adresse von number und erhöht direkt den Wert von number
  std::cout << "Number is: " << number << std::endl;

  // 4th case:
  reference_increment( number /* TODO */ );
  // Änderungen an arg sind direkte Änderungen an number
  // arg ist jetzt nur ein anderer Name für number
  // ++arg = ++number → number = 3
  std::cout << "Number is: " << number << std::endl;

  return 0;
}

