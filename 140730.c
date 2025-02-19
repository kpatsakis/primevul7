iscsi_co_generic_cb(struct iscsi_context *iscsi, int status,
                        void *command_data, void *opaque)
{
    struct IscsiTask *iTask = opaque;
    struct scsi_task *task = command_data;

    iTask->status = status;
    iTask->do_retry = 0;
    iTask->task = task;

    if (status != SCSI_STATUS_GOOD) {
        if (iTask->retries++ < ISCSI_CMD_RETRIES) {
            if (status == SCSI_STATUS_BUSY ||
                status == SCSI_STATUS_TIMEOUT ||
                status == SCSI_STATUS_TASK_SET_FULL) {
                unsigned retry_time =
                    exp_random(iscsi_retry_times[iTask->retries - 1]);
                if (status == SCSI_STATUS_TIMEOUT) {
                    /* make sure the request is rescheduled AFTER the
                     * reconnect is initiated */
                    retry_time = EVENT_INTERVAL * 2;
                    iTask->iscsilun->request_timed_out = true;
                }
                error_report("iSCSI Busy/TaskSetFull/TimeOut"
                             " (retry #%u in %u ms): %s",
                             iTask->retries, retry_time,
                             iscsi_get_error(iscsi));
                aio_timer_init(iTask->iscsilun->aio_context,
                               &iTask->retry_timer, QEMU_CLOCK_REALTIME,
                               SCALE_MS, iscsi_retry_timer_expired, iTask);
                timer_mod(&iTask->retry_timer,
                          qemu_clock_get_ms(QEMU_CLOCK_REALTIME) + retry_time);
                iTask->do_retry = 1;
            }
        } else if (status == SCSI_STATUS_CHECK_CONDITION) {
            int error = iscsi_translate_sense(&task->sense);
            if (error == EAGAIN) {
                error_report("iSCSI CheckCondition: %s",
                             iscsi_get_error(iscsi));
                iTask->do_retry = 1;
            } else {
                iTask->err_code = -error;
                iTask->err_str = g_strdup(iscsi_get_error(iscsi));
            }
        }
    }

    if (iTask->co) {
        replay_bh_schedule_oneshot_event(iTask->iscsilun->aio_context,
                                         iscsi_co_generic_bh_cb, iTask);
    } else {
        iTask->complete = 1;
    }
}