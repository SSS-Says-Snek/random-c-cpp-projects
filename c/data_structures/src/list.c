#include "data_structures/list.h"
#include <stdlib.h>

int List_new(List *list, size_t sizeof_type) {  // Probably will make it return List* w/ malloc instead of... yeah
    void **elems = malloc(sizeof_type);
    if (!elems) {
        return 1;
    }

    list->type_size = sizeof_type;
    list->size = 0;
    list->alloc_size = 1;
    list->elems = elems;

    return 0;
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
}
