static int megasas_dcmd_pd_get_list(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_pd_list info;
    size_t dcmd_size = sizeof(info);
    BusChild *kid;
    uint32_t offset, dcmd_limit, num_pd_disks = 0, max_pd_disks;

    memset(&info, 0, dcmd_size);
    offset = 8;
    dcmd_limit = offset + sizeof(struct mfi_pd_address);
    if (cmd->iov_size < dcmd_limit) {
        trace_megasas_dcmd_invalid_xfer_len(cmd->index, cmd->iov_size,
                                            dcmd_limit);
        return MFI_STAT_INVALID_PARAMETER;
    }

    max_pd_disks = (cmd->iov_size - offset) / sizeof(struct mfi_pd_address);
    if (max_pd_disks > MFI_MAX_SYS_PDS) {
        max_pd_disks = MFI_MAX_SYS_PDS;
    }
    QTAILQ_FOREACH(kid, &s->bus.qbus.children, sibling) {
        SCSIDevice *sdev = SCSI_DEVICE(kid->child);
        uint16_t pd_id;

        if (num_pd_disks >= max_pd_disks)
            break;

        pd_id = ((sdev->id & 0xFF) << 8) | (sdev->lun & 0xFF);
        info.addr[num_pd_disks].device_id = cpu_to_le16(pd_id);
        info.addr[num_pd_disks].encl_device_id = 0xFFFF;
        info.addr[num_pd_disks].encl_index = 0;
        info.addr[num_pd_disks].slot_number = sdev->id & 0xFF;
        info.addr[num_pd_disks].scsi_dev_type = sdev->type;
        info.addr[num_pd_disks].connect_port_bitmap = 0x1;
        info.addr[num_pd_disks].sas_addr[0] =
            cpu_to_le64(megasas_get_sata_addr(pd_id));
        num_pd_disks++;
        offset += sizeof(struct mfi_pd_address);
    }
    trace_megasas_dcmd_pd_get_list(cmd->index, num_pd_disks,
                                   max_pd_disks, offset);

    info.size = cpu_to_le32(offset);
    info.count = cpu_to_le32(num_pd_disks);

    cmd->iov_size -= dma_buf_read((uint8_t *)&info, offset, &cmd->qsg);
    return MFI_STAT_OK;
}