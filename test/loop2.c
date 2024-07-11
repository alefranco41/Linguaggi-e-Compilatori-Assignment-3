#include <stdio.h>
void foo(int a, int b) {
    int c = a * 2;
    body:  
        b = c + 5;
        if(a > 100){
            goto exit;
        }
        a++;
        goto body; 
    
    exit:
        printf("%d%d\n",a,b);
}

int main() {
  foo(1, 2);
  return 0;
}