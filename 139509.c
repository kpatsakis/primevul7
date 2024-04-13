vg_virgl_get_num_capsets(void)
{
    uint32_t capset2_max_ver, capset2_max_size;
    virgl_renderer_get_cap_set(VIRTIO_GPU_CAPSET_VIRGL2,
                               &capset2_max_ver,
                               &capset2_max_size);

    return capset2_max_ver ? 2 : 1;
}