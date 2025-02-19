static int megasas_dcmd_set_properties(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_ctrl_props info;
    size_t dcmd_size = sizeof(info);

    if (cmd->iov_size < dcmd_size) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            dcmd_size);
        return MFI_STAT_INVALID_PARAMETER;
    }
    dma_buf_write((uint8_t *)&info, dcmd_size, &cmd->qsg);
    trace_megasas_dcmd_unsupported(cmd->index, cmd->iov_size);
    return MFI_STAT_OK;
}