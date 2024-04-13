static int megasas_dcmd_pd_get_info(MegasasState *s, MegasasCmd *cmd)
{
    size_t dcmd_size = sizeof(struct mfi_pd_info);
    uint16_t pd_id;
    uint8_t target_id, lun_id;
    SCSIDevice *sdev = NULL;
    int retval = MFI_STAT_DEVICE_NOT_FOUND;

    if (cmd->iov_size < dcmd_size) {
        return MFI_STAT_INVALID_PARAMETER;
    }

    /* mbox0 has the ID */
    pd_id = le16_to_cpu(cmd->frame->dcmd.mbox[0]);
    target_id = (pd_id >> 8) & 0xFF;
    lun_id = pd_id & 0xFF;
    sdev = scsi_device_find(&s->bus, 0, target_id, lun_id);
    trace_megasas_dcmd_pd_get_info(cmd->index, pd_id);

    if (sdev) {
        /* Submit inquiry */
        retval = megasas_pd_get_info_submit(sdev, pd_id, cmd);
    }

    return retval;
}