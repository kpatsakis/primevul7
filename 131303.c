string_buffer_getsegcount(PyStringObject *self, Py_ssize_t *lenp)
{
    if ( lenp )
        *lenp = Py_SIZE(self);
    return 1;
}