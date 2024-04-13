iscsi_aio_cancel(BlockAIOCB *blockacb)
{
    IscsiAIOCB *acb = (IscsiAIOCB *)blockacb;
    IscsiLun *iscsilun = acb->iscsilun;

    qemu_mutex_lock(&iscsilun->mutex);

    /* If it was cancelled or completed already, our work is done here */
    if (acb->cancelled || acb->status != -EINPROGRESS) {
        qemu_mutex_unlock(&iscsilun->mutex);
        return;
    }

    acb->cancelled = true;

    qemu_aio_ref(acb); /* released in iscsi_abort_task_cb() */

    /* send a task mgmt call to the target to cancel the task on the target */
    if (iscsi_task_mgmt_abort_task_async(iscsilun->iscsi, acb->task,
                                         iscsi_abort_task_cb, acb) < 0) {
        qemu_aio_unref(acb); /* since iscsi_abort_task_cb() won't be called */
    }

    qemu_mutex_unlock(&iscsilun->mutex);
}