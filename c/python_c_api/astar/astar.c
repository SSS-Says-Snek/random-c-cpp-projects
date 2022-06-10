#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "astar/utils.h"

static PyObject *test_priority_queue(PyObject *self) {
    PriorityQueue priority_queue = {0, 1, malloc(1 * sizeof(AlgoQueueInfo))};
  
    for (int i = 0; i < 50; i++) {
        PriorityQueue_put(&priority_queue, (AlgoQueueInfo){i, 1, (Node){1, (Position){1, 1}}});
    }
    
    printf("Priorities in order of which has been popped first:\n");
    for (int i = 0; i < 50; i++) {
        AlgoQueueInfo result = PriorityQueue_pop(&priority_queue);
        printf("Priority: %i, Queue Size: %i, Queue Allocated Size: %i\n", result.f_score, priority_queue.size, priority_queue.max_size);
    }

    Py_RETURN_NONE;
}

static PyObject *algorithm(PyObject *self, PyObject *args) { //, PyObject *kwargs) {
    PyObject *list;

    const char *kwlist = {"path_list"};
    
    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }
    // if (!PyArg_ParseTupleAndKeywords(args, kwargs, "O", kwlist, &list)) {
    //     return NULL;
    // }

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

    for (Py_ssize_t i = 0; i < list_dimensions.height; i++) {
        for (Py_ssize_t j = 0; j < list_dimensions.width; j++) {
            printf("%i", node_array[i][j].value);
        }
    }

    for (Py_ssize_t i = 0; i < list_dimensions.height; i++) {
        free(node_array[i]);
    }
    free(node_array);

    return PyLong_FromLong(0L);

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
    return NULL;
}

// PYTHON METHOD AND MODULE STUFF

static PyMethodDef astar_methods[] = {
    {"algorithm", algorithm, METH_VARARGS, "test"},
    {"test_priority_queue", test_priority_queue, METH_NOARGS, "test"},
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