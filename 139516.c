virgl_cmd_context_destroy(VuGpu *g,
                          struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_ctx_destroy cd;

    VUGPU_FILL_CMD(cd);

    virgl_renderer_context_destroy(cd.hdr.ctx_id);
}