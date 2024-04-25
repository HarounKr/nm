#include <stdio.h>

typedef struct {
    int id;
    char name[20];
} Person;

int main() {
    Person person = {1, "John Doe"};
    printf("Person ID: %d, Name: %s\n", person.id, person.name);
    return 0;
}