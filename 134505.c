static int megasas_dcmd_get_fw_time(MegasasState *s, MegasasCmd *cmd)
{
    uint64_t fw_time;
    size_t dcmd_size = sizeof(fw_time);

    fw_time = cpu_to_le64(megasas_fw_time());

    cmd->iov_size -= dma_buf_read((uint8_t *)&fw_time, dcmd_size, &cmd->qsg);
    return MFI_STAT_OK;
}