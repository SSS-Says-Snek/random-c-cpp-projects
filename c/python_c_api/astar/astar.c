#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "astar/utils.h"
#include "astar/priorityqueue.h"

static PyObject *test_priority_queue(PyObject *self) {
    PriorityQueue priority_queue = {0, 1, malloc(1 * sizeof(AlgoQueueInfo))};
  
    for (int i = 0; i < 50; i++) {
        PriorityQueue_put(&priority_queue, (AlgoQueueInfo){i, 1, (Node){1, (Position){1, 1}}});
    }
    
    printf("Priorities in order of which has been popped first:\n");
    for (int i = 0; i < 50; i++) {
        AlgoQueueInfo result;

        int error_code = PriorityQueue_pop(&priority_queue, &result);
        if (error_code != 0) {
            return NULL;
        }
        printf("Priority: %i, Queue Size: %i, Queue Allocated Size: %i\n", result.f_score, priority_queue.size, priority_queue.max_size);
    }

    AlgoQueueInfo result;
    int e = PriorityQueue_pop(&priority_queue, &result);
    if (e == 2) {
        printf("AMOGUS");
    }

    Py_RETURN_NONE;
}

static PyObject *test_reconstruct_path(PyObject *self, PyObject *args) {
    PyObject *path_outputted;
    PyObject *start_node;
    PyObject *end_node;

    int error_code = 0;

    if (!PyArg_ParseTuple(args, "OOO", &path_outputted, &start_node, &end_node)) {
        return NULL;
    }

    return reconstruct_path(path_outputted, start_node, end_node, &error_code);
}

static PyObject *algorithm(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject *list;
    PyObject *start_pos;
    PyObject *end_pos;

    static char *kwlist[] = {"path_list", "start_pos", "end_pos"};
    
    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "OOO", kwlist,
        &list, &start_pos, &end_pos
    )) {
        return NULL;
    }

    int error_code = 0;
    Node **node_array = list_to_array_nodes(list, &error_code);
    
    // Handle error codes
    if (error_code != 0) {
        switch (error_code) {
            case 1:
                goto err_1;
            case 2:
                goto err_2;
            case 3:
                goto err_3;
            case 4:
                goto err_malloc;
            case 5:
                goto err_malloc;
        }
    }

    ListDimensions list_dimensions = get_list_dimensions(list);

    int count = 0;

    Node start_node = node_array[PyLong_AsLong(PyTuple_GetItem(start_pos, 1))][PyLong_AsLong(PyTuple_GetItem(start_pos, 0))];
    Node end_node = node_array[PyLong_AsLong(PyTuple_GetItem(end_pos, 1))][PyLong_AsLong(PyTuple_GetItem(end_pos, 0))];

    /*if (0 <= start_node.position.x && start_node.position.x < list_dimensions.width &&
        0 <= start_node.position.y && start_node.position.y < list_dimensions.height) {
    }*/

    PyObject *start_node_tuple = node_to_tuple(start_node);
    PyObject *end_node_tuple = node_to_tuple(end_node);

    PriorityQueue open_set;
    PriorityQueue_new(&open_set);
    PriorityQueue_put(&open_set, (AlgoQueueInfo){0, 0, start_node});

    PyObject *came_from = Py_BuildValue("{}");
    PyObject *g_score = Py_BuildValue("{}");
    PyObject *f_score = Py_BuildValue("{}");
    PyObject *open_set_hash = PySet_New(Py_BuildValue("[O]", start_node_tuple));

    Py_INCREF(open_set_hash);

    for (Py_ssize_t i = 0; i < list_dimensions.height; i++) {
        for (Py_ssize_t j = 0; j < list_dimensions.width; j++) {
            PyObject *node_tuple = node_to_tuple(node_array[i][j]);
            PyDict_SetItem(f_score, node_tuple, PyLong_FromLong(-1L));
            PyDict_SetItem(g_score, node_tuple, PyLong_FromLong(-1L));
        }
    }

    PyDict_SetItem(g_score, start_node_tuple, PyLong_FromLong(0L));
    PyDict_SetItem(f_score, start_node_tuple, PyFloat_FromDouble(manhattan(start_node.position, end_node.position)));

    while (!PriorityQueue_empty(open_set)) {
        AlgoQueueInfo algo_queue_info;
        PriorityQueue_pop(&open_set, &algo_queue_info);

        Node current_node = algo_queue_info.neighbor_node;
        Position current_pos = current_node.position;
        PyObject *current_node_tuple = node_to_tuple(current_node);
        PySet_Discard(open_set_hash, current_node_tuple);

        if (check_node_equal(current_node, end_node)) {
            // change node tup to pos

            for (Py_ssize_t i = 0; i < list_dimensions.height; i++) {
                free(node_array[i]);
            }
            free(node_array);

            PriorityQueue_destroy(&open_set);

            // Py_DECREF(start_node_tuple);
            // Py_DECREF(end_node_tuple);

            Py_DECREF(g_score);
            Py_DECREF(f_score);
            Py_DECREF(open_set_hash);
            Py_DECREF(current_node_tuple);

            int error_code;
            return reconstruct_path(came_from, PyTuple_GetItem(start_node_tuple, 1), PyTuple_GetItem(end_node_tuple, 1), &error_code);


        }
        
        int num_neighbors;
        Node *neighbors = get_neighbors(node_array, list_dimensions, current_pos, &num_neighbors);

        for (int i = 0; i < num_neighbors; i++) {
            Node neighbor_node = neighbors[i];
            PyObject *neighbor_node_tuple = node_to_tuple(neighbor_node);
            PyObject *current_g_score = PyDict_GetItem(g_score, current_node_tuple);
            PyObject *neighbor_g_score = PyDict_GetItem(g_score, neighbor_node_tuple);

            Py_INCREF(current_g_score);
            Py_INCREF(neighbor_g_score);

            PyObject *temp_g_score = PyNumber_Add(
                current_g_score,
                PyLong_FromLong(1L)
            );
            PyObject *temp_f_score = PyNumber_Add(
                temp_g_score, 
                PyFloat_FromDouble(manhattan(neighbor_node.position, end_node.position))
            );

            if (PyLong_AsLong(PyNumber_Subtract(temp_g_score, neighbor_g_score)) < 0L || PyLong_AsLong(neighbor_g_score) == -1L) {
                PyDict_SetItem(came_from, neighbor_node_tuple, current_node_tuple);

                PyDict_SetItem(
                    f_score, neighbor_node_tuple, 
                    PyNumber_Add(temp_g_score, PyFloat_FromDouble(manhattan(neighbor_node.position, end_node.position)))
                );
                PyDict_SetItem(
                    g_score, neighbor_node_tuple,
                    temp_g_score
                );

                if (!PySet_Contains(open_set_hash, neighbor_node_tuple)) {
                    count++;
                    PriorityQueue_put(&open_set, (AlgoQueueInfo){PyFloat_AsDouble(temp_f_score), count, neighbor_node});
                    PySet_Add(open_set_hash, neighbor_node_tuple);
                }
            }
            Py_DECREF(current_g_score);
            Py_DECREF(neighbor_g_score);

            Py_DECREF(neighbor_node_tuple);
        }
        
        Py_DECREF(current_node_tuple);
        free(neighbors);
    }

    // Free node array
    for (Py_ssize_t i = 0; i < list_dimensions.height; i++) {
        free(node_array[i]);
    }
    free(node_array);

    Py_RETURN_NONE;

// ERROR HANDLING

err_1:
    PyErr_SetString(PyExc_TypeError, "argument is not a list");
    return NULL;
err_2:
    PyErr_SetString(PyExc_ValueError, "list is empty");
    return NULL;
err_3:
    PyErr_SetString(PyExc_ValueError, "list is not an IxJ 2d list");
    return NULL;
err_malloc:
    PyErr_SetString(PyExc_MemoryError, "insufficient memory");
    return NULL;
}

// PYTHON METHOD AND MODULE STUFF

static PyMethodDef astar_methods[] = {
    {"algorithm", algorithm, METH_VARARGS | METH_KEYWORDS, "test"},
    {"test_priority_queue", test_priority_queue, METH_NOARGS, "test"},
    {"test_reconstruct_path", test_reconstruct_path, METH_VARARGS, "test"},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef astar_module = {
    PyModuleDef_HEAD_INIT,
    "astar",
    "A C implementation of A*", 
    -1,
    astar_methods
};

PyMODINIT_FUNC PyInit_astar() {
    Py_Initialize();
    return PyModule_Create(&astar_module);
}