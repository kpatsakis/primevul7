static union mfi_sgl *megasas_sgl_next(MegasasCmd *cmd,
                                       union mfi_sgl *sgl)
{
    uint8_t *next = (uint8_t *)sgl;

    if (megasas_frame_is_ieee_sgl(cmd)) {
        next += sizeof(struct mfi_sg_skinny);
    } else if (megasas_frame_is_sgl64(cmd)) {
        next += sizeof(struct mfi_sg64);
    } else {
        next += sizeof(struct mfi_sg32);
    }

    if (next >= (uint8_t *)cmd->frame + cmd->pa_size) {
        return NULL;
    }
    return (union mfi_sgl *)next;
}