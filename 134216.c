z2save(i_ctx_t *i_ctx_p)
{
    if (!save_page_device(igs))
        return zsave(i_ctx_p);
    return push_callout(i_ctx_p, "%savepagedevice");
}