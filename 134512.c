static uint64_t megasas_sgl_get_addr(MegasasCmd *cmd,
                                     union mfi_sgl *sgl)
{
    uint64_t addr;

    if (megasas_frame_is_ieee_sgl(cmd)) {
        addr = le64_to_cpu(sgl->sg_skinny->addr);
    } else if (megasas_frame_is_sgl64(cmd)) {
        addr = le64_to_cpu(sgl->sg64->addr);
    } else {
        addr = le32_to_cpu(sgl->sg32->addr);
    }
    return addr;
}