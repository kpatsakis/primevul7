return_self(PyStringObject *self)
{
    if (PyString_CheckExact(self)) {
        Py_INCREF(self);
        return self;
    }
    return (PyStringObject *)PyString_FromStringAndSize(
        PyString_AS_STRING(self),
        PyString_GET_SIZE(self));
}