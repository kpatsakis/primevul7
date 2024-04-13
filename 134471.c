static int megasas_mfc_get_defaults(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_defaults info;
    size_t dcmd_size = sizeof(struct mfi_defaults);

    memset(&info, 0x0, dcmd_size);
    if (cmd->iov_size < dcmd_size) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            dcmd_size);
        return MFI_STAT_INVALID_PARAMETER;
    }

    info.sas_addr = cpu_to_le64(s->sas_addr);
    info.stripe_size = 3;
    info.flush_time = 4;
    info.background_rate = 30;
    info.allow_mix_in_enclosure = 1;
    info.allow_mix_in_ld = 1;
    info.direct_pd_mapping = 1;
    /* Enable for BIOS support */
    info.bios_enumerate_lds = 1;
    info.disable_ctrl_r = 1;
    info.expose_enclosure_devices = 1;
    info.disable_preboot_cli = 1;
    info.cluster_disable = 1;

    cmd->iov_size -= dma_buf_read((uint8_t *)&info, dcmd_size, &cmd->qsg);
    return MFI_STAT_OK;
}