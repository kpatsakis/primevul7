zcurrentstackprotect(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    ref *ep = oparray_find(i_ctx_p);

    if (ep == 0)
        return_error(gs_error_rangecheck);
    push(1);
    make_bool(op, ep->value.opproc == oparray_cleanup);
    return 0;
}