#pragma once

#define PY_SSIZE_T_CLEAN
#include "Python.h"

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
    double f_score; // basically the priority
    int count;
    Node neighbor_node;
} AlgoQueueInfo;

int check_list_square(PyObject *list);
int check_node_equal(Node node1, Node node2);

double manhattan(Position pos_1, Position pos_2);
double euclidean(Position pos_1, Position pos_2);

ListDimensions get_list_dimensions(PyObject *list);

Node *get_neighbors(Node **node_array, ListDimensions node_array_dimensions, Position pos, int *num_neighbors);
Node **list_to_array_nodes(PyObject *list, int *error_code);

PyObject *node_to_tuple(Node node);
PyObject *reconstruct_path(PyObject *path_outputted, PyObject *start_tuple, PyObject *end_tuple, int *error_code);