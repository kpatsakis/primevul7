string_buffer_getcharbuf(PyStringObject *self, Py_ssize_t index, const char **ptr)
{
    if ( index != 0 ) {
        PyErr_SetString(PyExc_SystemError,
                        "accessing non-existent string segment");
        return -1;
    }
    *ptr = self->ob_sval;
    return Py_SIZE(self);
}