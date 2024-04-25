// Test de variables globales et locales
#include <stdio.h>

int globalVar = 10;

int add(int a, int b) {
    return a + b;
}

int main() {
    int localVar = 5;
    printf("Result: %d\n", add(globalVar, localVar));
    return 0;
}