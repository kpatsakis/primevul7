zsetstackprotect(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    ref *ep = oparray_find(i_ctx_p);

    check_type(*op, t_boolean);
    if (ep == 0)
        return_error(gs_error_rangecheck);
    ep->value.opproc =
        (op->value.boolval ? oparray_cleanup : oparray_no_cleanup);
    pop(1);
    return 0;
}