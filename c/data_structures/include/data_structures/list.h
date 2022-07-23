#include <stddef.h>

typedef struct {
    size_t type_size; // sizeof
    size_t size; // # of elements
    size_t alloc_size; // # allocated # of elements?
    void **elems;
} List;

List *List_new(size_t sizeof_type);
int List_append(List *list, void *elem);
void *List_get(List *list, size_t index);
void List_free(List *list);