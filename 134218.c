z2gstate(i_ctx_t *i_ctx_p)
{
    if (!save_page_device(igs))
        return zgstate(i_ctx_p);
    return push_callout(i_ctx_p, "%gstatepagedevice");
}