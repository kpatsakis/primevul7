void manager_check_finished(Manager *m) {
        char userspace[FORMAT_TIMESPAN_MAX], initrd[FORMAT_TIMESPAN_MAX], kernel[FORMAT_TIMESPAN_MAX], sum[FORMAT_TIMESPAN_MAX];
        usec_t firmware_usec, loader_usec, kernel_usec, initrd_usec, userspace_usec, total_usec;

        assert(m);

        if (m->n_running_jobs == 0)
                m->jobs_in_progress_event_source = sd_event_source_unref(m->jobs_in_progress_event_source);

        if (hashmap_size(m->jobs) > 0) {
                if (m->jobs_in_progress_event_source) {
                        uint64_t next = now(CLOCK_MONOTONIC) + JOBS_IN_PROGRESS_WAIT_USEC;
                        sd_event_source_set_time(m->jobs_in_progress_event_source, next);
                }
                return;
        }

        manager_flip_auto_status(m, false);

        /* Notify Type=idle units that we are done now */
        m->idle_pipe_event_source = sd_event_source_unref(m->idle_pipe_event_source);
        manager_close_idle_pipe(m);

        /* Turn off confirm spawn now */
        m->confirm_spawn = false;

        if (dual_timestamp_is_set(&m->finish_timestamp))
                return;

        dual_timestamp_get(&m->finish_timestamp);

        if (m->running_as == SYSTEMD_SYSTEM && detect_container(NULL) <= 0) {

                /* Note that m->kernel_usec.monotonic is always at 0,
                 * and m->firmware_usec.monotonic and
                 * m->loader_usec.monotonic should be considered
                 * negative values. */

                firmware_usec = m->firmware_timestamp.monotonic - m->loader_timestamp.monotonic;
                loader_usec = m->loader_timestamp.monotonic - m->kernel_timestamp.monotonic;
                userspace_usec = m->finish_timestamp.monotonic - m->userspace_timestamp.monotonic;
                total_usec = m->firmware_timestamp.monotonic + m->finish_timestamp.monotonic;

                if (dual_timestamp_is_set(&m->initrd_timestamp)) {

                        kernel_usec = m->initrd_timestamp.monotonic - m->kernel_timestamp.monotonic;
                        initrd_usec = m->userspace_timestamp.monotonic - m->initrd_timestamp.monotonic;

                        if (!log_on_console())
                                log_struct(LOG_INFO,
                                           MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                                           "KERNEL_USEC="USEC_FMT, kernel_usec,
                                           "INITRD_USEC="USEC_FMT, initrd_usec,
                                           "USERSPACE_USEC="USEC_FMT, userspace_usec,
                                           "MESSAGE=Startup finished in %s (kernel) + %s (initrd) + %s (userspace) = %s.",
                                           format_timespan(kernel, sizeof(kernel), kernel_usec, USEC_PER_MSEC),
                                           format_timespan(initrd, sizeof(initrd), initrd_usec, USEC_PER_MSEC),
                                           format_timespan(userspace, sizeof(userspace), userspace_usec, USEC_PER_MSEC),
                                           format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC),
                                           NULL);
                } else {
                        kernel_usec = m->userspace_timestamp.monotonic - m->kernel_timestamp.monotonic;
                        initrd_usec = 0;

                        if (!log_on_console())
                                log_struct(LOG_INFO,
                                           MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                                           "KERNEL_USEC="USEC_FMT, kernel_usec,
                                           "USERSPACE_USEC="USEC_FMT, userspace_usec,
                                           "MESSAGE=Startup finished in %s (kernel) + %s (userspace) = %s.",
                                           format_timespan(kernel, sizeof(kernel), kernel_usec, USEC_PER_MSEC),
                                           format_timespan(userspace, sizeof(userspace), userspace_usec, USEC_PER_MSEC),
                                           format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC),
                                           NULL);
                }
        } else {
                firmware_usec = loader_usec = initrd_usec = kernel_usec = 0;
                total_usec = userspace_usec = m->finish_timestamp.monotonic - m->userspace_timestamp.monotonic;

                if (!log_on_console())
                        log_struct(LOG_INFO,
                                   MESSAGE_ID(SD_MESSAGE_STARTUP_FINISHED),
                                   "USERSPACE_USEC="USEC_FMT, userspace_usec,
                                   "MESSAGE=Startup finished in %s.",
                                   format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC),
                                   NULL);
        }

        bus_manager_send_finished(m, firmware_usec, loader_usec, kernel_usec, initrd_usec, userspace_usec, total_usec);

        sd_notifyf(false,
                   "READY=1\nSTATUS=Startup finished in %s.",
                   format_timespan(sum, sizeof(sum), total_usec, USEC_PER_MSEC));
}