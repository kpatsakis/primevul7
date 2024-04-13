zstring(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    byte *sbody;
    uint size;

    check_type(*op, t_integer);
    if (op->value.intval < 0 )
        return_error(gs_error_rangecheck);
    if (op->value.intval > max_string_size )
        return_error(gs_error_limitcheck); /* to match Distiller */
    size = op->value.intval;
    sbody = ialloc_string(size, "string");
    if (sbody == 0)
        return_error(gs_error_VMerror);
    make_string(op, a_all | icurrent_space, size, sbody);
    memset(sbody, 0, size);
    return 0;
}