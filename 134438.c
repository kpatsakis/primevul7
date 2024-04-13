static int megasas_ctrl_shutdown(MegasasState *s, MegasasCmd *cmd)
{
    s->fw_state = MFI_FWSTATE_READY;
    return MFI_STAT_OK;
}