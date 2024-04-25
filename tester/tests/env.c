// Gestion de l'environnement
#include <stdio.h>
#include <stdlib.h>

int main() {
    char *server = getenv("LOGNAME");
    if (server == NULL) {
        printf("Logname environment variable is not set.\n");
    } else {
        printf("Log name: %s\n", server);
    }
    return 0;
}
