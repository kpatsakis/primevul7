qemuProcessQEMULabelUniqPath(qemuProcessQMPPtr proc)
{
    /* We cannot use the security driver here, but we should not need to. */
    if (chown(proc->uniqDir, proc->runUid, -1) < 0) {
        virReportSystemError(errno,
                             _("Cannot chown uniq path: %s"),
                             proc->uniqDir);
        return -1;
    }

    return 0;
}