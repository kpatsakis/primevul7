ExprList_Append(ExprList *l, expr_ty exp)
{
    ExprList_check_invariants(l);
    if (l->size >= l->allocated) {
        /* We need to alloc (or realloc) the memory. */
        Py_ssize_t new_size = l->allocated * 2;

        /* See if we've ever allocated anything dynamically. */
        if (l->p == l->data) {
            Py_ssize_t i;
            /* We're still using the cached data. Switch to
               alloc-ing. */
            l->p = PyMem_RawMalloc(sizeof(expr_ty) * new_size);
            if (!l->p)
                return -1;
            /* Copy the cached data into the new buffer. */
            for (i = 0; i < l->size; i++)
                l->p[i] = l->data[i];
        } else {
            /* Just realloc. */
            expr_ty *tmp = PyMem_RawRealloc(l->p, sizeof(expr_ty) * new_size);
            if (!tmp) {
                PyMem_RawFree(l->p);
                l->p = NULL;
                return -1;
            }
            l->p = tmp;
        }

        l->allocated = new_size;
        assert(l->allocated == 2 * l->size);
    }

    l->p[l->size++] = exp;

    ExprList_check_invariants(l);
    return 0;
}