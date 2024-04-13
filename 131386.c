string_partition(PyStringObject *self, PyObject *sep_obj)
{
    const char *sep;
    Py_ssize_t sep_len;

    if (PyString_Check(sep_obj)) {
        sep = PyString_AS_STRING(sep_obj);
        sep_len = PyString_GET_SIZE(sep_obj);
    }
#ifdef Py_USING_UNICODE
    else if (PyUnicode_Check(sep_obj))
        return PyUnicode_Partition((PyObject *) self, sep_obj);
#endif
    else if (PyObject_AsCharBuffer(sep_obj, &sep, &sep_len))
        return NULL;

    return stringlib_partition(
        (PyObject*) self,
        PyString_AS_STRING(self), PyString_GET_SIZE(self),
        sep_obj, sep, sep_len
        );
}