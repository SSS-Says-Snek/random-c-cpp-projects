#include "data_structures/list.h"
#include <stdlib.h>
#include <stdio.h>

List *List_new() {
    List *list = malloc(sizeof(List));

    if (!list) {
        return NULL;
    }

    void **elems = malloc(sizeof(void *));
    if (!elems) {
        return NULL;
    }

    list->size = 0;
    list->alloc_size = 1;
    list->elems = elems;

    return list;
}

int List_append(List *list, void *element) {
    if (!list->elems) {
        return 1;
    }

    if (list->size == list->alloc_size) {
        list->alloc_size *= 2; // alloc factor

        list->elems = realloc(list->elems, list->alloc_size * sizeof(*(list->elems)));

        if (!list->elems) {
            return 1;
        }
    }

    // printf("%i", list->size);

    list->elems[list->size] = element;
    list->size++;

    return 0;
}

int List_clear(List *list) {
    list->elems = realloc(list->elems, 0);
    if (!list->elems) {
        return 1;
    }

    list->size = 0;
    list->alloc_size = 1;

    return 0;
}

void *List_get(List *list, size_t index) {
    return list->elems[index];
}

void List_free(List *list) {
    for (size_t i = 0; i < list->size; i++) {
        free(list->elems[i]);
    }

    free(list->elems);
    list->elems = NULL;

    free(list);
    list = NULL;
}
