z2gsave(i_ctx_t *i_ctx_p)
{
    if (!save_page_device(igs))
        return gs_gsave(igs);
    return push_callout(i_ctx_p, "%gsavepagedevice");
}