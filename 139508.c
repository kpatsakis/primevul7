virgl_cmd_set_scanout(VuGpu *g,
                      struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_set_scanout ss;
    struct virgl_renderer_resource_info info;
    int ret;

    VUGPU_FILL_CMD(ss);

    if (ss.scanout_id >= VIRTIO_GPU_MAX_SCANOUTS) {
        g_critical("%s: illegal scanout id specified %d",
                   __func__, ss.scanout_id);
        cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_SCANOUT_ID;
        return;
    }

    memset(&info, 0, sizeof(info));

    if (ss.resource_id && ss.r.width && ss.r.height) {
        ret = virgl_renderer_resource_get_info(ss.resource_id, &info);
        if (ret == -1) {
            g_critical("%s: illegal resource specified %d\n",
                       __func__, ss.resource_id);
            cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
            return;
        }

        int fd = -1;
        if (virgl_renderer_get_fd_for_texture(info.tex_id, &fd) < 0) {
            g_critical("%s: failed to get fd for texture\n", __func__);
            cmd->error = VIRTIO_GPU_RESP_ERR_INVALID_RESOURCE_ID;
            return;
        }
        assert(fd >= 0);
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_DMABUF_SCANOUT,
            .size = sizeof(VhostUserGpuDMABUFScanout),
            .payload.dmabuf_scanout.scanout_id = ss.scanout_id,
            .payload.dmabuf_scanout.x =  ss.r.x,
            .payload.dmabuf_scanout.y =  ss.r.y,
            .payload.dmabuf_scanout.width = ss.r.width,
            .payload.dmabuf_scanout.height = ss.r.height,
            .payload.dmabuf_scanout.fd_width = info.width,
            .payload.dmabuf_scanout.fd_height = info.height,
            .payload.dmabuf_scanout.fd_stride = info.stride,
            .payload.dmabuf_scanout.fd_flags = info.flags,
            .payload.dmabuf_scanout.fd_drm_fourcc = info.drm_fourcc
        };
        vg_send_msg(g, &msg, fd);
        close(fd);
    } else {
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_DMABUF_SCANOUT,
            .size = sizeof(VhostUserGpuDMABUFScanout),
            .payload.dmabuf_scanout.scanout_id = ss.scanout_id,
        };
        g_debug("disable scanout");
        vg_send_msg(g, &msg, -1);
    }
    g->scanout[ss.scanout_id].resource_id = ss.resource_id;
}