// Manipulation de fichiers
#include <stdio.h>

int main() {
    FILE *file;
    file = fopen("example.txt", "w+");
    if (file == NULL) {
        return 1; // File not opened error
    }
    fprintf(file, "Hello, File!\n");
    fclose(file);
    printf("File operations done.\n");
    return 0;
}