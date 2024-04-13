iscsi_abort_task_cb(struct iscsi_context *iscsi, int status, void *command_data,
                    void *private_data)
{
    IscsiAIOCB *acb = private_data;

    /* If the command callback hasn't been called yet, drop the task */
    if (!acb->bh) {
        /* Call iscsi_aio_ioctl_cb() with SCSI_STATUS_CANCELLED */
        iscsi_scsi_cancel_task(iscsi, acb->task);
    }

    qemu_aio_unref(acb); /* acquired in iscsi_aio_cancel() */
}