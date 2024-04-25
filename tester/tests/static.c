#include <stdio.h>
// Test avec des static
static int staticVariable = 10;

static void staticFunction() {
    printf("This is a static function.\n");
}

int main() {
    staticFunction();
    printf("%d\n", staticVariable);
    return 0;
}