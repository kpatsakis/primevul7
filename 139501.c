virgl_cmd_context_create(VuGpu *g,
                         struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_ctx_create cc;

    VUGPU_FILL_CMD(cc);

    virgl_renderer_context_create(cc.hdr.ctx_id, cc.nlen,
                                  cc.debug_name);
}