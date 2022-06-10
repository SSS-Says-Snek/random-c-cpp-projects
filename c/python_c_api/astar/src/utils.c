#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <math.h>
#include "astar/utils.h"

#define get_nested_list(list, i, j) PyList_GetItem(PyList_GetItem(list, i), j)

const int SOLID_TILES = {1};

void PriorityQueue_put(PriorityQueue *queue, AlgoQueueInfo algo_queue_info) {
    if (queue->size + 1 > queue->max_size) {
        queue->max_size *= 2;

        queue->queue = realloc(queue->queue, queue->max_size * sizeof(AlgoQueueInfo));
    }
    
    int i = queue->size - 1;
    int f_score = algo_queue_info.f_score;

    while (i >= 0 && (f_score >= queue->queue[i].f_score || f_score == -1)) {
        queue->queue[i + 1] = queue->queue[i];
        i--;
    }

    queue->queue[i + 1] = algo_queue_info;
    queue->size++;
}

AlgoQueueInfo PriorityQueue_pop(PriorityQueue *queue) {
    // No handling for when queue is empty!! will implement very soon, but sleep

    AlgoQueueInfo result = queue->queue[queue->size - 1];

    queue->size--;

    if (queue->size > 0 && queue->max_size / queue->size == 2) {
        queue->queue = realloc(queue->queue, queue->size * sizeof(AlgoQueueInfo));
        queue->max_size /= 2;
    }

    return result;
}

int check_list_square(PyObject *list) {
    if (!PyList_Check(list)) {
        return 1;
    }

    if (PyList_Size(list) == 0) {
        return 2;
    }

    Py_ssize_t ideal_width = PyList_Size(PyList_GetItem(list, 0));
    
    for (Py_ssize_t i = 0; i < ideal_width; i++) {
        if (PyList_Size(PyList_GetItem(list, i)) != ideal_width) {
            return 3;
        }
    }
    return 0;
}

double manhattan(Position pos_1, Position pos_2) {
    return fabs(pos_2.x - pos_1.x) + fabs(pos_2.y - pos_1.y);
}

double euclidean(Position pos_1, Position pos_2) {
    return sqrt(pow(pos_2.x - pos_1.x, 2) + pow(pos_2.y - pos_1.y, 2));
}

ListDimensions get_list_dimensions(PyObject *list) {
    ListDimensions list_dimensions = {PyList_Size(PyList_GetItem(list, 0)), PyList_Size(list)};
    return list_dimensions;
}

Node *get_neighbors(Node **node_array, ListDimensions node_array_dimensions, Position pos) {
    int num_neighbors = 0;
    Node *neighbors = malloc(4 * sizeof(Node));  // 4 max, realloc to num_neighbors afterwards

    Position left = {pos.x - 1, pos.y};
    Position right = {pos.x + 1, pos.y};
    Position up = {pos.x, pos.y - 1};
    Position down = {pos.x, pos.y + 1};
    Position neighbor_idxs[4] = {left, right, up, down};

    for (int i = 0; i < 4; i++) {
        Position neighbor_idx = neighbor_idxs[i];

        if (neighbor_idx.x >= 0 && neighbor_idx.x <= node_array_dimensions.width - 1 &&
            neighbor_idx.y >= 0 && neighbor_idx.y <= node_array_dimensions.height - 1) {
            
            Node neighbor = node_array[neighbor_idx.y][neighbor_idx.x];
            neighbors[num_neighbors] = neighbor;

            num_neighbors++;
        }
    }

    return neighbors;
}

Node **list_to_array_nodes(PyObject *list, int *error_code) {
    int check_result = check_list_square(list);
    if (check_result != 0) {
        *error_code = check_result;
        return NULL;
    }

    Py_ssize_t list_height = PyList_Size(list);
    Py_ssize_t list_width = PyList_Size(PyList_GetItem(list, 0));

    // Initialize and populate 2d array
    Node **multi_array;
    multi_array = malloc(list_height * sizeof(*multi_array));

    if (!multi_array) {
        *error_code = 4;
        return NULL;
    }

    for (Py_ssize_t i = 0; i < list_height; i++) {
        multi_array[i] = malloc(list_width * sizeof(Node));

        if (!multi_array[i]) {
            *error_code = 5;
            free(multi_array);
            return NULL;
        }
        
        for (Py_ssize_t j = 0; j < list_width; j++) {
            Position temp_pos = {i, j};
            Node temp_node = {PyLong_AsLong(get_nested_list(list, i, j)), temp_pos};

            multi_array[i][j] = temp_node;
        }
    }

    return multi_array;
}
