vg_virgl_init(VuGpu *g)
{
    int ret;

    if (g->drm_rnode_fd && virgl_cbs.version == 1) {
        g_warning("virgl will use the default rendernode");
    }

    ret = virgl_renderer_init(g,
                              VIRGL_RENDERER_USE_EGL |
                              VIRGL_RENDERER_THREAD_SYNC,
                              &virgl_cbs);
    if (ret != 0) {
        return false;
    }

    ret = virgl_renderer_get_poll_fd();
    if (ret != -1) {
        g->renderer_source =
            vug_source_new(&g->dev, ret, G_IO_IN, vg_virgl_poll, g);
    }

    return true;
}