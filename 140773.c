iscsi_set_events(IscsiLun *iscsilun)
{
    struct iscsi_context *iscsi = iscsilun->iscsi;
    int ev = iscsi_which_events(iscsi);

    if (ev != iscsilun->events) {
        aio_set_fd_handler(iscsilun->aio_context, iscsi_get_fd(iscsi),
                           false,
                           (ev & POLLIN) ? iscsi_process_read : NULL,
                           (ev & POLLOUT) ? iscsi_process_write : NULL,
                           NULL,
                           iscsilun);
        iscsilun->events = ev;
    }
}