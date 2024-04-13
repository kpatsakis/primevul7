do_argstrip(PyStringObject *self, int striptype, PyObject *args)
{
    PyObject *sep = NULL;

    if (!PyArg_ParseTuple(args, (char *)stripformat[striptype], &sep))
        return NULL;

    if (sep != NULL && sep != Py_None) {
        if (PyString_Check(sep))
            return do_xstrip(self, striptype, sep);
#ifdef Py_USING_UNICODE
        else if (PyUnicode_Check(sep)) {
            PyObject *uniself = PyUnicode_FromObject((PyObject *)self);
            PyObject *res;
            if (uniself==NULL)
                return NULL;
            res = _PyUnicode_XStrip((PyUnicodeObject *)uniself,
                striptype, sep);
            Py_DECREF(uniself);
            return res;
        }
#endif
        PyErr_Format(PyExc_TypeError,
#ifdef Py_USING_UNICODE
                     "%s arg must be None, str or unicode",
#else
                     "%s arg must be None or str",
#endif
                     STRIPNAME(striptype));
        return NULL;
    }

    return do_strip(self, striptype);
}