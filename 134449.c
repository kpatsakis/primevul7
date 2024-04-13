static int megasas_dcmd_get_properties(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_ctrl_props info;
    size_t dcmd_size = sizeof(info);

    memset(&info, 0x0, dcmd_size);
    if (cmd->iov_size < dcmd_size) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            dcmd_size);
        return MFI_STAT_INVALID_PARAMETER;
    }
    info.pred_fail_poll_interval = cpu_to_le16(300);
    info.intr_throttle_cnt = cpu_to_le16(16);
    info.intr_throttle_timeout = cpu_to_le16(50);
    info.rebuild_rate = 30;
    info.patrol_read_rate = 30;
    info.bgi_rate = 30;
    info.cc_rate = 30;
    info.recon_rate = 30;
    info.cache_flush_interval = 4;
    info.spinup_drv_cnt = 2;
    info.spinup_delay = 6;
    info.ecc_bucket_size = 15;
    info.ecc_bucket_leak_rate = cpu_to_le16(1440);
    info.expose_encl_devices = 1;

    cmd->iov_size -= dma_buf_read((uint8_t *)&info, dcmd_size, &cmd->qsg);
    return MFI_STAT_OK;
}