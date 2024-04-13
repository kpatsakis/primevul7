string_subscript(PyStringObject* self, PyObject* item)
{
    if (PyIndex_Check(item)) {
        Py_ssize_t i = PyNumber_AsSsize_t(item, PyExc_IndexError);
        if (i == -1 && PyErr_Occurred())
            return NULL;
        if (i < 0)
            i += PyString_GET_SIZE(self);
        return string_item(self, i);
    }
    else if (PySlice_Check(item)) {
        Py_ssize_t start, stop, step, slicelength, cur, i;
        char* source_buf;
        char* result_buf;
        PyObject* result;

        if (_PySlice_Unpack(item, &start, &stop, &step) < 0) {
            return NULL;
        }
        slicelength = _PySlice_AdjustIndices(PyString_GET_SIZE(self), &start,
                                            &stop, step);

        if (slicelength <= 0) {
            return PyString_FromStringAndSize("", 0);
        }
        else if (start == 0 && step == 1 &&
                 slicelength == PyString_GET_SIZE(self) &&
                 PyString_CheckExact(self)) {
            Py_INCREF(self);
            return (PyObject *)self;
        }
        else if (step == 1) {
            return PyString_FromStringAndSize(
                PyString_AS_STRING(self) + start,
                slicelength);
        }
        else {
            source_buf = PyString_AsString((PyObject*)self);
            result_buf = (char *)PyMem_Malloc(slicelength);
            if (result_buf == NULL)
                return PyErr_NoMemory();

            for (cur = start, i = 0; i < slicelength;
                 cur += step, i++) {
                result_buf[i] = source_buf[cur];
            }

            result = PyString_FromStringAndSize(result_buf,
                                                slicelength);
            PyMem_Free(result_buf);
            return result;
        }
    }
    else {
        PyErr_Format(PyExc_TypeError,
                     "string indices must be integers, not %.200s",
                     Py_TYPE(item)->tp_name);
        return NULL;
    }
}