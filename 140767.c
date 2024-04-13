iscsi_process_write(void *arg)
{
    IscsiLun *iscsilun = arg;
    struct iscsi_context *iscsi = iscsilun->iscsi;

    qemu_mutex_lock(&iscsilun->mutex);
    iscsi_service(iscsi, POLLOUT);
    iscsi_set_events(iscsilun);
    qemu_mutex_unlock(&iscsilun->mutex);
}