string_buffer_getwritebuf(PyStringObject *self, Py_ssize_t index, const void **ptr)
{
    PyErr_SetString(PyExc_TypeError,
                    "Cannot use string as modifiable buffer");
    return -1;
}