    if (iTask.status != SCSI_STATUS_GOOD) {
        iscsi_allocmap_set_invalid(iscsilun, sector_num * BDRV_SECTOR_SIZE,
                                   nb_sectors * BDRV_SECTOR_SIZE);
        error_report("iSCSI WRITE10/16 failed at lba %" PRIu64 ": %s", lba,
                     iTask.err_str);
        r = iTask.err_code;
        goto out_unlock;
    }