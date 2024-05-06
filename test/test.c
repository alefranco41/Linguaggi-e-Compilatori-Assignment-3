#include <stdio.h>

int main() {
    int n;
    printf("Inserisci un numero intero positivo: ");
    scanf("%d", &n);

    int i;
    int sum = 0;

    for (i = 1; i <= n; i++) {
        sum += i;
    }

    printf("La somma dei primi %d numeri interi positivi è: %d\n", n, sum);

    return 0;
}