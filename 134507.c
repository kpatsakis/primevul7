static int megasas_dcmd_dummy(MegasasState *s, MegasasCmd *cmd)
{
    trace_megasas_dcmd_dummy(cmd->index, cmd->iov_size);
    return MFI_STAT_OK;
}