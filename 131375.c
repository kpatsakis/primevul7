PyString_Size(register PyObject *op)
{
    if (!PyString_Check(op))
        return string_getsize(op);
    return Py_SIZE(op);
}