virgl_cmd_resource_unref(VuGpu *g,
                         struct virtio_gpu_ctrl_command *cmd)
{
    struct virtio_gpu_resource_unref unref;
    struct iovec *res_iovs = NULL;
    int num_iovs = 0;

    VUGPU_FILL_CMD(unref);

    virgl_renderer_resource_detach_iov(unref.resource_id,
                                       &res_iovs,
                                       &num_iovs);
    g_free(res_iovs);

    virgl_renderer_resource_unref(unref.resource_id);
}