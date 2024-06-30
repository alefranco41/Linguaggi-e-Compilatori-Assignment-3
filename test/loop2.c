#include <stdio.h>

void foo(int a, int b) {
    int c = a * 2;
    if(c > 0){
        body:
        a++;
        b = c + 5;
        if(a > 10){
            goto exit;
        }
        goto body;
    }   
    
    exit:
        printf("%d%d\n",a,b);
}

int main() {
  foo(1, 2);
  return 0;
}