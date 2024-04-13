zbytestring(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    byte *sbody;
    uint size;

    check_int_leu(*op, max_int);
    size = (uint)op->value.intval;
    sbody = ialloc_bytes(size, ".bytestring");
    if (sbody == 0)
        return_error(gs_error_VMerror);
    make_astruct(op, a_all | icurrent_space, sbody);
    memset(sbody, 0, size);
    return 0;
}