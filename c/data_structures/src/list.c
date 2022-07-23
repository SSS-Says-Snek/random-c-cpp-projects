#include "data_structures/list.h"
#include <stdlib.h>
#include <stdio.h>

List *List_new(size_t sizeof_type) {
    List *list = malloc(sizeof(List));


    void **elems = malloc(sizeof_type);
    if (!elems) {
        return NULL;
    }

    list->type_size = sizeof_type;
    list->size = 0;
    list->alloc_size = 1;
    list->elems = elems;

    return list;
}

int List_append(List *list, void *element) {
    if (!list->elems) {
        return 1;
    }

    if (list->size + 1 > list->alloc_size) {
        list->alloc_size *= 2; // alloc factor

        list->elems = realloc(list->elems, list->alloc_size * list->type_size);

        if (!list->elems) {
            return 1;
        }
    }

    // printf("%i", list->size);

    list->elems[list->size] = element;
    list->size++;

    return 0;
}

void *List_get(List *list, size_t index) {
    return list->elems[index];
}

void List_free(List *list) {
    free(list->elems);
    list->elems = NULL;

    free(list);
    list = NULL;
}
