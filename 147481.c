qemuProcessLookupPTYs(virDomainChrDefPtr *devices,
                      int count,
                      GHashTable *info)
{
    size_t i;

    for (i = 0; i < count; i++) {
        g_autofree char *id = NULL;
        virDomainChrDefPtr chr = devices[i];
        if (chr->source->type == VIR_DOMAIN_CHR_TYPE_PTY) {
            qemuMonitorChardevInfoPtr entry;

            id = g_strdup_printf("char%s", chr->info.alias);

            entry = virHashLookup(info, id);
            if (!entry || !entry->ptyPath) {
                if (chr->source->data.file.path == NULL) {
                    /* neither the log output nor 'info chardev' had a
                     * pty path for this chardev, report an error
                     */
                    virReportError(VIR_ERR_INTERNAL_ERROR,
                                   _("no assigned pty for device %s"), id);
                    return -1;
                } else {
                    /* 'info chardev' had no pty path for this chardev,
                     * but the log output had, so we're fine
                     */
                    continue;
                }
            }

            g_free(chr->source->data.file.path);
            chr->source->data.file.path = g_strdup(entry->ptyPath);
        }
    }

    return 0;
}