static int megasas_dcmd_ld_get_info(MegasasState *s, MegasasCmd *cmd)
{
    struct mfi_ld_info info;
    size_t dcmd_size = sizeof(info);
    uint16_t ld_id;
    uint32_t max_ld_disks = s->fw_luns;
    SCSIDevice *sdev = NULL;
    int retval = MFI_STAT_DEVICE_NOT_FOUND;

    if (cmd->iov_size < dcmd_size) {
        return MFI_STAT_INVALID_PARAMETER;
    }

    /* mbox0 has the ID */
    ld_id = le16_to_cpu(cmd->frame->dcmd.mbox[0]);
    trace_megasas_dcmd_ld_get_info(cmd->index, ld_id);

    if (megasas_is_jbod(s)) {
        return MFI_STAT_DEVICE_NOT_FOUND;
    }

    if (ld_id < max_ld_disks) {
        sdev = scsi_device_find(&s->bus, 0, ld_id, 0);
    }

    if (sdev) {
        retval = megasas_ld_get_info_submit(sdev, ld_id, cmd);
    }

    return retval;
}