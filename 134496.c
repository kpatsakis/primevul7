static void megasas_finish_dcmd(MegasasCmd *cmd, uint32_t iov_size)
{
    trace_megasas_finish_dcmd(cmd->index, iov_size);

    if (iov_size > cmd->iov_size) {
        if (megasas_frame_is_ieee_sgl(cmd)) {
            cmd->frame->dcmd.sgl.sg_skinny->len = cpu_to_le32(iov_size);
        } else if (megasas_frame_is_sgl64(cmd)) {
            cmd->frame->dcmd.sgl.sg64->len = cpu_to_le32(iov_size);
        } else {
            cmd->frame->dcmd.sgl.sg32->len = cpu_to_le32(iov_size);
        }
    }
}