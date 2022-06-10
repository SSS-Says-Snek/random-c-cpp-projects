#pragma once

typedef struct {
    Py_ssize_t width;
    Py_ssize_t height;
} ListDimensions;

typedef struct {
    Py_ssize_t x;
    Py_ssize_t y;
} Position;

typedef struct {
    int value;
    Position position;
} Node;

typedef struct {
    int f_score; // basically the priority
    int count;
    Node neighbor_node;
} AlgoQueueInfo;

typedef struct {
    int size;
    int max_size;
    AlgoQueueInfo *queue;
} PriorityQueue;

void PriorityQueue_put(PriorityQueue *queue, AlgoQueueInfo algo_queue_info);
AlgoQueueInfo PriorityQueue_pop(PriorityQueue *queue);

int check_list_square(PyObject *list);

double manhattan(Position pos_1, Position pos_2);
double euclidean(Position pos_1, Position pos_2);

ListDimensions get_list_dimensions(PyObject *list);

Node *get_neighbors(Node **node_array, ListDimensions node_array_dimensions, Position pos);
Node **list_to_array_nodes(PyObject *list, int *error_code);