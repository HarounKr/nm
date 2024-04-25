// Test avec une foncion inline
#include <stdio.h>

static inline void inlineFunction() {
    printf("This is an inline function.\n");
}

int main() {
    inlineFunction();
    return 0;
}
