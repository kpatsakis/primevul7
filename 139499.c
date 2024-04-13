virgl_cmd_ctx_detach_resource(VuGpu *g,
                              struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_ctx_resource det_res;

    VUGPU_FILL_CMD(det_res);

    virgl_renderer_ctx_detach_resource(det_res.hdr.ctx_id, det_res.resource_id);
}