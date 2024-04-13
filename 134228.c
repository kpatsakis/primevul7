z2copy_gstate(i_ctx_t *i_ctx_p)
{
    if (!save_page_device(igs))
        return zcopy_gstate(i_ctx_p);
    return push_callout(i_ctx_p, "%copygstatepagedevice");
}