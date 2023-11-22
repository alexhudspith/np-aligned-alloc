/*
 * Python extension module for Numpy aligned memory allocator.
 */
#define NPY_TARGET_VERSION NPY_1_24_API_VERSION
#define NPY_NO_DEPRECATED_API NPY_TARGET_VERSION

// Include Python.h before any standard headers
// https://docs.python.org/3/extending/extending.html
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "numpy/arrayobject.h"

#include "np_aligned_alloc.h"

PyDataMem_Handler np_aligned_alloc_mem_handler = {
    "np_aligned_alloc_mem_handler",
    1,
    {
        NULL,
        np_malloc,
        np_calloc,
        np_realloc,
        np_free
    }
};

PyObject *enable(PyObject *Py_UNUSED(mod), PyObject *Py_UNUSED(args)) {
    // Numpy looks specifically for a capsule named "mem_handler"
    PyObject *new_handler = PyCapsule_New(&np_aligned_alloc_mem_handler, "mem_handler", NULL);
    if (new_handler == NULL) {
        // Exception is set
        return NULL;
    }

    PyObject *old_handler = PyDataMem_SetHandler(new_handler);
    if (old_handler == NULL) {
        Py_DECREF(new_handler);
        if (!PyErr_Occurred()) {
            PyErr_NoMemory();
        }
        // Exception is set
        return NULL;
    }

    Py_DECREF(old_handler);
    Py_RETURN_NONE;
}

PyObject *disable(PyObject *Py_UNUSED(mod), PyObject *Py_UNUSED(args)) {
    PyObject *old_handler = PyDataMem_SetHandler(NULL);
    if (old_handler == NULL) {
        if (!PyErr_Occurred()) {
            PyErr_NoMemory();
        }
        // Exception is set
        return NULL;
    }

    Py_DECREF(old_handler);
    Py_RETURN_NONE;
}

static PyMethodDef module_functions[] = {
	{"enable", enable, METH_NOARGS, NULL},
	{"disable", disable, METH_NOARGS, NULL},
	{NULL, NULL, 0, NULL},
};

static PyModuleDef module_def = {
    PyModuleDef_HEAD_INIT,
    .m_name = "np_aligned_alloc",
    .m_methods = module_functions,
};

PyMODINIT_FUNC PyInit_np_aligned_alloc(void) {
    debug("init");
    import_array();

    return PyModuleDef_Init(&module_def);
}
