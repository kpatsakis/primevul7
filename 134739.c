zfinderrorobject(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    ref errobj;

    if (errorexec_find(i_ctx_p, &errobj)) {
        push(2);
        op[-1] = errobj;
        make_true(op);
    } else {
        push(1);
        make_false(op);
    }
    return 0;
}