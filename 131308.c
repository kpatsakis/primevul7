string__format__(PyObject* self, PyObject* args)
{
    PyObject *format_spec;
    PyObject *result = NULL;
    PyObject *tmp = NULL;

    /* If 2.x, convert format_spec to the same type as value */
    /* This is to allow things like u''.format('') */
    if (!PyArg_ParseTuple(args, "O:__format__", &format_spec))
        goto done;
    if (!(PyString_Check(format_spec) || PyUnicode_Check(format_spec))) {
        PyErr_Format(PyExc_TypeError, "__format__ arg must be str "
                     "or unicode, not %s", Py_TYPE(format_spec)->tp_name);
        goto done;
    }
    tmp = PyObject_Str(format_spec);
    if (tmp == NULL)
        goto done;
    format_spec = tmp;

    result = _PyBytes_FormatAdvanced(self,
                                     PyString_AS_STRING(format_spec),
                                     PyString_GET_SIZE(format_spec));
done:
    Py_XDECREF(tmp);
    return result;
}