static bool manager_get_show_status(Manager *m) {
        assert(m);

        if (m->running_as != SYSTEMD_SYSTEM)
                return false;

        if (m->no_console_output)
                return false;

        if (m->show_status > 0)
                return true;

        /* If Plymouth is running make sure we show the status, so
         * that there's something nice to see when people press Esc */

        return plymouth_running();
}