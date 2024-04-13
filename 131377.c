string_buffer_getreadbuf(PyStringObject *self, Py_ssize_t index, const void **ptr)
{
    if ( index != 0 ) {
        PyErr_SetString(PyExc_SystemError,
                        "accessing non-existent string segment");
        return -1;
    }
    *ptr = (void *)self->ob_sval;
    return Py_SIZE(self);
}