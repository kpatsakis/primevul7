static int megasas_event_info(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_evt_log_state info;
    size_t dcmd_size = sizeof(info);

    memset(&info, 0, dcmd_size);

    info.newest_seq_num = cpu_to_le32(s->event_count);
    info.shutdown_seq_num = cpu_to_le32(s->shutdown_event);
    info.boot_seq_num = cpu_to_le32(s->boot_event);

    cmd->iov_size -= dma_buf_read((uint8_t *)&info, dcmd_size, &cmd->qsg);
    return MFI_STAT_OK;
}