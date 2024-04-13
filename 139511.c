virgl_cmd_resource_flush(VuGpu *g,
                         struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_resource_flush rf;
    int i;

    VUGPU_FILL_CMD(rf);

    glFlush();
    if (!rf.resource_id) {
        g_debug("bad resource id for flush..?");
        return;
    }
    for (i = 0; i < VIRTIO_GPU_MAX_SCANOUTS; i++) {
        if (g->scanout[i].resource_id != rf.resource_id) {
            continue;
        }
        VhostUserGpuMsg msg = {
            .request = VHOST_USER_GPU_DMABUF_UPDATE,
            .size = sizeof(VhostUserGpuUpdate),
            .payload.update.scanout_id = i,
            .payload.update.x = rf.r.x,
            .payload.update.y = rf.r.y,
            .payload.update.width = rf.r.width,
            .payload.update.height = rf.r.height
        };
        vg_send_msg(g, &msg, -1);
        vg_wait_ok(g);
    }
}