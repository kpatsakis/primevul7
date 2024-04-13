string_contains(PyObject *str_obj, PyObject *sub_obj)
{
    if (!PyString_CheckExact(sub_obj)) {
#ifdef Py_USING_UNICODE
        if (PyUnicode_Check(sub_obj))
            return PyUnicode_Contains(str_obj, sub_obj);
#endif
        if (!PyString_Check(sub_obj)) {
            PyErr_Format(PyExc_TypeError,
                "'in <string>' requires string as left operand, "
                "not %.200s", Py_TYPE(sub_obj)->tp_name);
            return -1;
        }
    }

    return stringlib_contains_obj(str_obj, sub_obj);
}