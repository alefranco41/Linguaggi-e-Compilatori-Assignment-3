#include <stdio.h>

void foo(int n) {
  int a = 0;
  int b = 10;
  int c = 20;
  int x = 5;
  
  for(int i=x; i<100; i++){
    a = b + c; //l'istruzione è loop invariant ma non domina tutti gli usi all'interno dell loop
  }

  printf("%d\n", a);
}

int main() {
  foo(100);
  return 0;
}