string_translate(PyStringObject *self, PyObject *args)
{
    register char *input, *output;
    const char *table;
    register Py_ssize_t i, c, changed = 0;
    PyObject *input_obj = (PyObject*)self;
    const char *output_start, *del_table=NULL;
    Py_ssize_t inlen, tablen, dellen = 0;
    PyObject *result;
    int trans_table[256];
    PyObject *tableobj, *delobj = NULL;

    if (!PyArg_UnpackTuple(args, "translate", 1, 2,
                          &tableobj, &delobj))
        return NULL;

    if (PyString_Check(tableobj)) {
        table = PyString_AS_STRING(tableobj);
        tablen = PyString_GET_SIZE(tableobj);
    }
    else if (tableobj == Py_None) {
        table = NULL;
        tablen = 256;
    }
#ifdef Py_USING_UNICODE
    else if (PyUnicode_Check(tableobj)) {
        /* Unicode .translate() does not support the deletechars
           parameter; instead a mapping to None will cause characters
           to be deleted. */
        if (delobj != NULL) {
            PyErr_SetString(PyExc_TypeError,
            "deletions are implemented differently for unicode");
            return NULL;
        }
        return PyUnicode_Translate((PyObject *)self, tableobj, NULL);
    }
#endif
    else if (PyObject_AsCharBuffer(tableobj, &table, &tablen))
        return NULL;

    if (tablen != 256) {
        PyErr_SetString(PyExc_ValueError,
          "translation table must be 256 characters long");
        return NULL;
    }

    if (delobj != NULL) {
        if (PyString_Check(delobj)) {
            del_table = PyString_AS_STRING(delobj);
            dellen = PyString_GET_SIZE(delobj);
        }
#ifdef Py_USING_UNICODE
        else if (PyUnicode_Check(delobj)) {
            PyErr_SetString(PyExc_TypeError,
            "deletions are implemented differently for unicode");
            return NULL;
        }
#endif
        else if (PyObject_AsCharBuffer(delobj, &del_table, &dellen))
            return NULL;
    }
    else {
        del_table = NULL;
        dellen = 0;
    }

    inlen = PyString_GET_SIZE(input_obj);
    result = PyString_FromStringAndSize((char *)NULL, inlen);
    if (result == NULL)
        return NULL;
    output_start = output = PyString_AsString(result);
    input = PyString_AS_STRING(input_obj);

    if (dellen == 0 && table != NULL) {
        /* If no deletions are required, use faster code */
        for (i = inlen; --i >= 0; ) {
            c = Py_CHARMASK(*input++);
            if (Py_CHARMASK((*output++ = table[c])) != c)
                changed = 1;
        }
        if (changed || !PyString_CheckExact(input_obj))
            return result;
        Py_DECREF(result);
        Py_INCREF(input_obj);
        return input_obj;
    }

    if (table == NULL) {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(i);
    } else {
        for (i = 0; i < 256; i++)
            trans_table[i] = Py_CHARMASK(table[i]);
    }

    for (i = 0; i < dellen; i++)
        trans_table[(int) Py_CHARMASK(del_table[i])] = -1;

    for (i = inlen; --i >= 0; ) {
        c = Py_CHARMASK(*input++);
        if (trans_table[c] != -1)
            if (Py_CHARMASK(*output++ = (char)trans_table[c]) == c)
                continue;
        changed = 1;
    }
    if (!changed && PyString_CheckExact(input_obj)) {
        Py_DECREF(result);
        Py_INCREF(input_obj);
        return input_obj;
    }
    /* Fix the size of the resulting string */
    if (inlen > 0 && _PyString_Resize(&result, output - output_start))
        return NULL;
    return result;
}