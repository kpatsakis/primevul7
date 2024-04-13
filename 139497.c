virgl_get_drm_fd(void *opaque)
{
    VuGpu *g = opaque;

    return g->drm_rnode_fd;
}