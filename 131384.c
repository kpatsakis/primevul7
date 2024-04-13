basestring_new(PyTypeObject *type, PyObject *args, PyObject *kwds)
{
    PyErr_SetString(PyExc_TypeError,
                    "The basestring type cannot be instantiated");
    return NULL;
}