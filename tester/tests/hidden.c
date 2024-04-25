#include <stdio.h>

__attribute__((visibility("hidden"))) void hiddenFunction() {
    printf("This function is hidden.\n");
}

int main() {
    hiddenFunction();
    return 0;
}