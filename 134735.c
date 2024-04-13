oparray_find(i_ctx_t *i_ctx_p)
{
    long i;
    ref *ep;

    for (i = 0; (ep = ref_stack_index(&e_stack, i)) != 0; ++i) {
        if (r_is_estack_mark(ep) &&
            (ep->value.opproc == oparray_cleanup ||
             ep->value.opproc == oparray_no_cleanup)
            )
            return ep;
    }
    return 0;
}