z2grestoreall(i_ctx_t *i_ctx_p)
{
    for (;;) {
        int code = restore_page_device(i_ctx_p, igs, gs_gstate_saved(igs));
        if (code < 0) return code;
        if (code == 0) {
            bool done = !gs_gstate_saved(gs_gstate_saved(igs));

            gs_grestore(igs);
            if (done)
                break;
        } else
            return push_callout(i_ctx_p, "%grestoreallpagedevice");
    }
    return 0;
}