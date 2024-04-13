_PyString_Join(PyObject *sep, PyObject *x)
{
    assert(sep != NULL && PyString_Check(sep));
    assert(x != NULL);
    return string_join((PyStringObject *)sep, x);
}