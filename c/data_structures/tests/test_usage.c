#include "data_structures/list.h"
#include <stdio.h>
#include <stdlib.h>

void amogus(List *list) {
    int sussy = 69;
    List_append(list, &sussy);
}

int *huh(int i) {
    int *sus = malloc(sizeof(int));

    *sus = i;

    return sus;
}


int main() {
    int g = 3;

    List *f = List_new();

    /*List_append(f, &g);
    List_append(f, &g);

    int *e = List_get(f, 1);

    printf("%i", *e);
    amogus(f);

    int *h = List_get(f, 2);
    printf("%i", *h);*/

    for (int i = 0; i <= 100; i++) {
        List_append(f, huh(i));
    }

    for (int i = 0; i <= 100; i++) {
        int *val = List_get(f, i);
        printf("%i", *val);
    }

    List_free(f);

    return 0;
}