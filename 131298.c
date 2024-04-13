string_sizeof(PyStringObject *v)
{
    Py_ssize_t res;
    res = PyStringObject_SIZE + PyString_GET_SIZE(v) * Py_TYPE(v)->tp_itemsize;
    return PyInt_FromSsize_t(res);
}