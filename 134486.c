static uint32_t megasas_sgl_get_len(MegasasCmd *cmd,
                                    union mfi_sgl *sgl)
{
    uint32_t len;

    if (megasas_frame_is_ieee_sgl(cmd)) {
        len = le32_to_cpu(sgl->sg_skinny->len);
    } else if (megasas_frame_is_sgl64(cmd)) {
        len = le32_to_cpu(sgl->sg64->len);
    } else {
        len = le32_to_cpu(sgl->sg32->len);
    }
    return len;
}