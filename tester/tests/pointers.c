// Pointeurs et allocation dynamique
#include <stdio.h>
#include <stdlib.h>

int main() {
    int *array = malloc(10 * sizeof(int));
    if (array == NULL) {
        return 1; // Failed allocation
    }
    for (int i = 0; i < 10; i++) {
        array[i] = i;
    }
    free(array);
    return 0;
}