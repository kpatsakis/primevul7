qemuProcessQMPInit(qemuProcessQMPPtr proc)
{
    g_autofree char *template = NULL;

    VIR_DEBUG("proc=%p, emulator=%s", proc, proc->binary);

    template = g_strdup_printf("%s/qmp-XXXXXX", proc->libDir);

    if (!(proc->uniqDir = g_mkdtemp(template))) {
        virReportSystemError(errno,
                             _("Failed to create unique directory with "
                               "template '%s' for probing QEMU"),
                             template);
        return -1;
    }
    /* if g_mkdtemp succeeds, proc->uniqDir is now the owner of
     * the string. Set template to NULL to avoid freeing
     * the memory in this case */
    template = NULL;

    if (qemuProcessQEMULabelUniqPath(proc) < 0)
        return -1;

    proc->monpath = g_strdup_printf("%s/%s", proc->uniqDir, "qmp.monitor");

    proc->monarg = g_strdup_printf("unix:%s,server,nowait", proc->monpath);

    /*
     * Normally we'd use runDir for pid files, but because we're using
     * -daemonize we need QEMU to be allowed to create them, rather
     * than libvirtd. So we're using libDir which QEMU can write to
     */
    proc->pidfile = g_strdup_printf("%s/%s", proc->uniqDir, "qmp.pid");

    return 0;
}