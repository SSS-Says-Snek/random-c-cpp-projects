#include "data_structures/list.h"
#include <stdio.h>


int main() {
    List f;

    int g = 3;

    List_new(&f, sizeof(int)); // Probably will make it return List* w/ malloc instead of... yeah

    List_append(&f, &g);
    List_append(&f, &g);

    int *e = List_get(&f, 1);

    printf("%i", *e);

    return 0;
}