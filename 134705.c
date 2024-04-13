zerrorexec(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;

    check_op(2);
    check_estack(4);            /* mark/cleanup, errobj, pop, obj */
    push_mark_estack(es_other, errorexec_cleanup);
    *++esp = op[-1];
    push_op_estack(errorexec_pop);
    code = zexec(i_ctx_p);
    if (code >= 0)
        pop(1);
    else
        esp -= 3;               /* undo our additions to estack */
    return code;
}