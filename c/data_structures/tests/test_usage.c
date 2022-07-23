#include "data_structures/list.h"
#include <stdio.h>

void amogus(List *list) {
    int sussy = 69;
    List_append(list, &sussy);
}


int main() {
    int g = 3;

    List *f = List_new(sizeof(int));

    List_append(f, &g);
    List_append(f, &g);

    int *e = List_get(f, 1);

    printf("%i", *e);
    amogus(f);

    int *h = List_get(f, 2);
    printf("%i", *h);

    List_free(f);

    return 0;
}