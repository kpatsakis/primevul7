z2copy(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code = zcopy(i_ctx_p);

    if (code >= 0)
        return code;
    if (!r_has_type(op, t_astruct))
        return code;
    return z2copy_gstate(i_ctx_p);
}