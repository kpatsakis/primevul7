static bool megasas_frame_is_sense64(MegasasCmd *cmd)
{
    return cmd->flags & MFI_FRAME_SENSE64;
}