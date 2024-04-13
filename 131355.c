PyString_AsStringAndSize(register PyObject *obj,
                         register char **s,
                         register Py_ssize_t *len)
{
    if (s == NULL) {
        PyErr_BadInternalCall();
        return -1;
    }

    if (!PyString_Check(obj)) {
#ifdef Py_USING_UNICODE
        if (PyUnicode_Check(obj)) {
            obj = _PyUnicode_AsDefaultEncodedString(obj, NULL);
            if (obj == NULL)
                return -1;
        }
        else
#endif
        {
            PyErr_Format(PyExc_TypeError,
                         "expected string or Unicode object, "
                         "%.200s found", Py_TYPE(obj)->tp_name);
            return -1;
        }
    }

    *s = PyString_AS_STRING(obj);
    if (len != NULL)
        *len = PyString_GET_SIZE(obj);
    else if (strlen(*s) != (size_t)PyString_GET_SIZE(obj)) {
        PyErr_SetString(PyExc_TypeError,
                        "expected string without null bytes");
        return -1;
    }
    return 0;
}