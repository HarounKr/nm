// Les symboles faibles et forts sont utiles dans des scénarios où plusieurs définitions d'une même fonction ou variable peuvent exister.
#include <stdio.h>

__attribute__((weak)) int weakVar = 5;
__attribute__((weak)) int weakFunction() {
    return weakVar;
}

int main() {
    printf("Weak Function Output: %d\n", weakFunction());
    return 0;
}
