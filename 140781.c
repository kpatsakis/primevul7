    if (iTask.status != SCSI_STATUS_GOOD) {
        error_report("iSCSI READ10/16 failed at lba %" PRIu64 ": %s",
                     lba, iTask.err_str);
        r = iTask.err_code;
    }