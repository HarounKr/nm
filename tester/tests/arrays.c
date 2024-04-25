// Manipulation de tableaux et boucles
#include <stdio.h>

#define SIZE 5

int main() {
    int numbers[SIZE] = {1, 2, 3, 4, 5};
    int sum = 0;
    for(int i = 0; i < SIZE; i++) {
        sum += numbers[i];
    }
    printf("Sum: %d\n", sum);
    return 0;
}