void manager_set_show_status(Manager *m, ShowStatus mode) {
        assert(m);
        assert(IN_SET(mode, SHOW_STATUS_AUTO, SHOW_STATUS_NO, SHOW_STATUS_YES, SHOW_STATUS_TEMPORARY));

        if (m->running_as != SYSTEMD_SYSTEM)
                return;

        m->show_status = mode;

        if (mode > 0)
                touch("/run/systemd/show-status");
        else
                unlink("/run/systemd/show-status");
}