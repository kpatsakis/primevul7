static int megasas_dcmd_set_fw_time(MegasasState *s, MegasasCmd *cmd)
{
    uint64_t fw_time;

    /* This is a dummy; setting of firmware time is not allowed */
    memcpy(&fw_time, cmd->frame->dcmd.mbox, sizeof(fw_time));

    trace_megasas_dcmd_set_fw_time(cmd->index, fw_time);
    fw_time = cpu_to_le64(megasas_fw_time());
    return MFI_STAT_OK;
}