#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define get_nested_list(list, i, j) PyList_GetItem(PyList_GetItem(list, i), j)

int _check_matrix_dimensions(PyObject *matrix_a, PyObject *matrix_b) {
    Py_ssize_t matrix_a_height = PyList_Size(matrix_a);
    Py_ssize_t matrix_b_height = PyList_Size(matrix_b);

    for (Py_ssize_t i = 0; i < matrix_a_height; i++) {
        if (PyList_Size(PyList_GetItem(matrix_a, i)) != matrix_a_height) {
            PyErr_SetString(PyExc_ValueError, "argument 1 matrix is not a square matrix");
            return -1;
        }
    }

    for (Py_ssize_t i = 0; i < matrix_b_height; i++) {
        if (PyList_Size(PyList_GetItem(matrix_b, i)) != matrix_b_height) {
            PyErr_SetString(PyExc_ValueError, "argument 2 matrix is not a square matrix");
            return -1;
        }
    }

    if (matrix_a_height != matrix_b_height) {
        PyErr_SetString(PyExc_ValueError, "matrices do not have same dimensions");
        return -1;
    }
    return 0;
}

long *_nested_list_to_pointer(PyObject *list) {
    Py_ssize_t list_size = PyList_Size(list);
    long *array_pointer = malloc((list_size * list_size) * sizeof(long));

    for (Py_ssize_t i = 0; i < list_size; i++) {
        for (Py_ssize_t j = 0; j < list_size; j++) {
            array_pointer[i * list_size + j] = PyLong_AsLong(get_nested_list(list, i, j));
        }
    }

    return array_pointer;
}

static PyObject *test_thing(PyObject *self, PyObject *args) {
    char *stuff = NULL;

    if (!PyArg_ParseTuple(args, "s", &stuff)) {
        return NULL;
    }
    printf("This is from C! %s\n", stuff);

    Py_RETURN_NONE;
}

static PyObject *add_for(PyObject *self, PyObject *args) {
    int num_loops;
    unsigned long long result = 0;

    if (!PyArg_ParseTuple(args, "i", &num_loops)) {
        return NULL;
    }

    for (int i = 0; i <= num_loops; i++) {
        result += i;
    };

    return PyLong_FromUnsignedLongLong(result);
}

static PyObject *test_keywords(PyObject *self, PyObject *args, PyObject *kwargs) {
    int a;
    int b;
    int c = 0;
    int d = 1;
    int e = 69;

    static char *kwlist[] = {"a", "b", "c", "d", "e", NULL};

    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "ii|iii", kwlist, 
        &a, &b, &c, &d, &e
    )) {
        return NULL;
    }

    return Py_BuildValue("iiiii", a, b, c, d, e);
}

static PyObject *test_list(PyObject *self, PyObject *args) {
    PyObject *list;

    if (!PyArg_ParseTuple(args, "O", &list)) {
        return NULL;
    }

    int total = 0;

    for (int i = 0; i < PyList_Size(list); i++) {
        PyObject *nested_list = PyList_GetItem(list, i);
        for (int j = 0; j < PyList_Size(nested_list); j++) {
            total += PyLong_AsLong(PyList_GetItem(nested_list, j));
        }
    }

    return PyLong_FromLong(total);
}

static PyObject *test_exception(PyObject *self, PyObject *args) {
    double dividend;
    double divisor;

    if (!PyArg_ParseTuple(args, "dd", &dividend, &divisor)) {
        return NULL;
    }

    if (divisor == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "division by zero");
        return NULL;
    }

    return PyFloat_FromDouble(dividend / divisor);
}

static PyObject *matrix_multiplication(PyObject *self, PyObject *args, PyObject *kwargs) {
    PyObject *matrix_a;
    PyObject *matrix_b;

    static char *kwlist[] = {"matrix_a", "matrix_b", NULL};

    if (!PyArg_ParseTupleAndKeywords(
        args, kwargs, "OO", kwlist,
        &matrix_a, &matrix_b
    )) {
        return NULL;
    }

    int result = _check_matrix_dimensions(matrix_a, matrix_b);
    if (result == -1) {
        return NULL;
    }

    long *matrix_a_pointer = _nested_list_to_pointer(matrix_a);
    long *matrix_b_pointer = _nested_list_to_pointer(matrix_b);

    Py_ssize_t n = PyList_Size(matrix_a); // assume that height == width

    // Initializes the output matrix and computational array
    long *computational_array = calloc(n * n, sizeof(long));
    PyObject *output_matrix = PyList_New(n);
    
    // Actual computation

    for (Py_ssize_t i = 0; i < n; i++) {
        for (Py_ssize_t j = 0; j < n; j++) {
            for (Py_ssize_t k = 0; k < n; k++) {
                computational_array[i * n + j] += (matrix_a_pointer[i * n + k] * matrix_b_pointer[k * n + j]);
            }
        }
    }

    // Return
    for (Py_ssize_t i = 0; i < n; i++) {
        PyObject *row = PyList_New(n);
        
        for (Py_ssize_t j = 0; j < n; j++) {
            PyList_SetItem(row, j, PyLong_FromLong(computational_array[i * n + j]));
        };
        PyList_SetItem(output_matrix, i, row);
    }

    computational_array = NULL;
    matrix_a_pointer = NULL;
    matrix_b_pointer = NULL;

    free(computational_array);
    free(matrix_a_pointer);
    free(matrix_b_pointer);

    return output_matrix;
}

// DOCSTRINGS

PyDoc_STRVAR(
    matrix_multiplication_doc,
    "matrix_multiplication(matrix_a: list, matrix_b: list) -> list[list[int]]\n"
    "\n"
    "    Multiplies two 2D lists. Matrices must both be of size NxN."
);

// Method and module defs

static PyMethodDef test_thing_methods[] = {
    {"test_thing", test_thing, METH_VARARGS, "Random docstring LLLLL"},
    {"add_for", add_for, METH_VARARGS, "So it adds....."},
    {"test_keywords", test_keywords, METH_VARARGS | METH_KEYWORDS, "Tests out keyword arg support"},
    {"test_list", test_list, METH_VARARGS, "Tests out (nested) list support"},
    {"test_exception", test_exception, METH_VARARGS, "Tests out Python exceptions in C"},
    {"matrix_multiplication", matrix_multiplication, METH_VARARGS | METH_KEYWORDS, matrix_multiplication_doc},
    {NULL, NULL, 0, NULL}
};

static struct PyModuleDef test_thing_module = {
    PyModuleDef_HEAD_INIT,
    "test_thing",
    "ANOTHER RANDOM DOCSTRING LLL", 
    -1,
    test_thing_methods
};

PyMODINIT_FUNC PyInit_test_thing() {
    Py_Initialize();
    return PyModule_Create(&test_thing_module);
}