z2setgstate(i_ctx_t *i_ctx_p)
{
    os_ptr op = osp;
    int code;

    check_stype(*op, st_igstate_obj);
    code = restore_page_device(i_ctx_p, igs, igstate_ptr(op));
    if (code < 0) return code;
    if (code == 0)
        return zsetgstate(i_ctx_p);
    return push_callout(i_ctx_p, "%setgstatepagedevice");
}