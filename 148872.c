make_str_node_and_del(PyObject **str, struct compiling *c, const node* n)
{
    PyObject *s = *str;
    PyObject *kind = make_str_kind(STR(CHILD(n, 0)));
    if (!kind) {
        return NULL;
    }
    *str = NULL;
    assert(PyUnicode_CheckExact(s));
    if (PyArena_AddPyObject(c->c_arena, s) < 0) {
        Py_DECREF(s);
        return NULL;
    }
    return Str(s, kind, LINENO(n), n->n_col_offset, c->c_arena);
}