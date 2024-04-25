#include <stdio.h>
#include <string.h>

int main() {
    const char src[50] = "This is source";
    char dest[50];
    strcpy(dest, src);
    printf("Copied string: %s\n", dest);
    return 0;
}