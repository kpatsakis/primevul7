static int megasas_dcmd_ld_list_query(MegasasState *s, MegasasCmd *cmd)
{
    uint16_t flags;
    struct mfi_ld_targetid_list info;
    size_t dcmd_size = sizeof(info), resid;
    uint32_t num_ld_disks = 0, max_ld_disks = s->fw_luns;
    BusChild *kid;

    /* mbox0 contains flags */
    flags = le16_to_cpu(cmd->frame->dcmd.mbox[0]);
    trace_megasas_dcmd_ld_list_query(cmd->index, flags);
    if (flags != MR_LD_QUERY_TYPE_ALL &&
        flags != MR_LD_QUERY_TYPE_EXPOSED_TO_HOST) {
        max_ld_disks = 0;
    }

    memset(&info, 0, dcmd_size);
    if (cmd->iov_size < 12) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            dcmd_size);
        return MFI_STAT_INVALID_PARAMETER;
    }
    dcmd_size = sizeof(uint32_t) * 2 + 3;
    max_ld_disks = cmd->iov_size - dcmd_size;
    if (megasas_is_jbod(s)) {
        max_ld_disks = 0;
    }
    if (max_ld_disks > MFI_MAX_LD) {
        max_ld_disks = MFI_MAX_LD;
    }
    QTAILQ_FOREACH(kid, &s->bus.qbus.children, sibling) {
        SCSIDevice *sdev = SCSI_DEVICE(kid->child);

        if (num_ld_disks >= max_ld_disks) {
            break;
        }
        info.targetid[num_ld_disks] = sdev->lun;
        num_ld_disks++;
        dcmd_size++;
    }
    info.ld_count = cpu_to_le32(num_ld_disks);
    info.size = dcmd_size;
    trace_megasas_dcmd_ld_get_list(cmd->index, num_ld_disks, max_ld_disks);

    resid = dma_buf_read((uint8_t *)&info, dcmd_size, &cmd->qsg);
    cmd->iov_size = dcmd_size - resid;
    return MFI_STAT_OK;
}