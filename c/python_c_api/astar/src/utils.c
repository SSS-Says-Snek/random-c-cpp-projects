#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <math.h>
#include "astar/utils.h"

#define get_nested_list(list, i, j) PyList_GetItem(PyList_GetItem(list, i), j)

const int SOLID_TILES[] = {1};
const int LEN_SOLID_TILES = sizeof(SOLID_TILES) / sizeof(SOLID_TILES[0]);

int check_list(PyObject *list) {
    if (!PyList_CheckExact(list)) {
        return 1;
    }

    if (PyList_Size(list) == 0) {
        return 2;
    }

    Py_ssize_t ideal_width = PyList_Size(PyList_GetItem(list, 0));
    
    for (Py_ssize_t i = 0; i < PyList_Size(list); i++) {
        if (PyList_Size(PyList_GetItem(list, i)) != ideal_width) {
            return 3;
        }
    }
    return 0;
}

int check_node_equal(Node node1, Node node2) {
    if (node1.value != node2.value) {
        return 0;
    } else if (node1.position.x != node2.position.x) {
        return 0;
    } else if (node1.position.y != node2.position.y) {
        return 0;
    }

    return 1;
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

Node *get_neighbors(Node **node_array, ListDimensions node_array_dimensions, Position pos, int *num_neighbors_ptr, int *error_code) {
    int num_neighbors = 0;
    Node *neighbors = malloc(4 * sizeof(Node));  // 4 max

    if (!neighbors) {
        *error_code = 1;
        return NULL;
    }

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
            
            int is_solid = 0;
            for (int tile_i = 0; tile_i < LEN_SOLID_TILES; tile_i++) {
                if (neighbor.value == SOLID_TILES[tile_i]) {
                    is_solid = 1;
                    break;
                }
            }
            if (is_solid) {
                continue;
            }
            neighbors[num_neighbors] = neighbor;

            num_neighbors++;
        }
    }

    *num_neighbors_ptr = num_neighbors;

    return neighbors;
}

Node **list_to_array_nodes(PyObject *list, int *error_code) {
    int check_result = check_list(list);
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

            for (Py_ssize_t j = 0; j < i - 1; j++) {
                free(multi_array[j]);
            }
            free(multi_array);

            multi_array = NULL;
            return NULL;
        }
        
        for (Py_ssize_t j = 0; j < list_width; j++) {
            Position temp_pos = {j, i};
            Node temp_node = {PyLong_AsLong(get_nested_list(list, i, j)), temp_pos};

            multi_array[i][j] = temp_node;
        }
    }

    return multi_array;
}

PyObject *node_to_tuple(Node node) {
    PyObject *tuple = Py_BuildValue("(i(nn))", node.value, node.position.x, node.position.y);
    return tuple;
}

PyObject *reconstruct_path(PyObject *path_outputted, PyObject *start_tuple, PyObject *end_tuple, int *error_code) {
    if (!PyDict_CheckExact(path_outputted)) {
        *error_code = 1;
    }

    PyObject *reconstructed_dict = Py_BuildValue("{}");
    PyObject *result = Py_BuildValue("[]");
    PyObject *path_outputted_items = PyDict_Items(path_outputted);

    for (Py_ssize_t i = 0; i < PyList_Size(path_outputted_items); i++) {
        PyObject *pair = PyList_GetItem(path_outputted_items, i);
        PyObject *key = PyTuple_GetItem(pair, 0);
        PyObject *value = PyTuple_GetItem(pair, 1);

        PyDict_SetItem(reconstructed_dict, PyTuple_GetItem(key, 1), PyTuple_GetItem(value, 1));
    }

    while (PyDict_Contains(reconstructed_dict, end_tuple)) {
        PyObject *new_end_tuple = Py_BuildValue("(nn)", PyLong_AsSsize_t(PyTuple_GetItem(end_tuple, 0)), PyLong_AsSsize_t(PyTuple_GetItem(end_tuple, 1)));
        PyList_Append(result, new_end_tuple);

        end_tuple = PyDict_GetItem(reconstructed_dict, end_tuple);
    }

    PyList_Append(result, start_tuple);
    PyList_Reverse(result);

    Py_DECREF(reconstructed_dict);
    Py_DECREF(path_outputted_items);

    return result;
}