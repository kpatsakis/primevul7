virgl_cmd_ctx_attach_resource(VuGpu *g,
                              struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_ctx_resource att_res;

    VUGPU_FILL_CMD(att_res);

    virgl_renderer_ctx_attach_resource(att_res.hdr.ctx_id, att_res.resource_id);
}