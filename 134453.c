static bool megasas_frame_is_ieee_sgl(MegasasCmd *cmd)
{
    return cmd->flags & MFI_FRAME_IEEE_SGL;
}