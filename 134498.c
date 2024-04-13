static bool megasas_frame_is_sgl64(MegasasCmd *cmd)
{
    return cmd->flags & MFI_FRAME_SGL64;
}