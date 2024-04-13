static int megasas_map_sgl(MegasasState *s, MegasasCmd *cmd, union mfi_sgl *sgl)
{
    int i;
    int iov_count = 0;
    size_t iov_size = 0;

    cmd->flags = le16_to_cpu(cmd->frame->header.flags);
    iov_count = cmd->frame->header.sge_count;
    if (iov_count > MEGASAS_MAX_SGE) {
        trace_megasas_iovec_sgl_overflow(cmd->index, iov_count,
                                         MEGASAS_MAX_SGE);
        return iov_count;
    }
    pci_dma_sglist_init(&cmd->qsg, PCI_DEVICE(s), iov_count);
    for (i = 0; i < iov_count; i++) {
        dma_addr_t iov_pa, iov_size_p;

        if (!sgl) {
            trace_megasas_iovec_sgl_underflow(cmd->index, i);
            goto unmap;
        }
        iov_pa = megasas_sgl_get_addr(cmd, sgl);
        iov_size_p = megasas_sgl_get_len(cmd, sgl);
        if (!iov_pa || !iov_size_p) {
            trace_megasas_iovec_sgl_invalid(cmd->index, i,
                                            iov_pa, iov_size_p);
            goto unmap;
        }
        qemu_sglist_add(&cmd->qsg, iov_pa, iov_size_p);
        sgl = megasas_sgl_next(cmd, sgl);
        iov_size += (size_t)iov_size_p;
    }
    if (cmd->iov_size > iov_size) {
        trace_megasas_iovec_overflow(cmd->index, iov_size, cmd->iov_size);
    } else if (cmd->iov_size < iov_size) {
        trace_megasas_iovec_underflow(cmd->iov_size, iov_size, cmd->iov_size);
    }
    cmd->iov_offset = 0;
    return 0;
unmap:
    qemu_sglist_destroy(&cmd->qsg);
    return iov_count - i;
}