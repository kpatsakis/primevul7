qemuProcessPrepareChardevDevice(virDomainDefPtr def G_GNUC_UNUSED,
                                virDomainChrDefPtr dev,
                                void *opaque G_GNUC_UNUSED)
{
    int fd;
    if (dev->source->type != VIR_DOMAIN_CHR_TYPE_FILE)
        return 0;

    if ((fd = open(dev->source->data.file.path,
                   O_CREAT | O_APPEND, S_IRUSR|S_IWUSR)) < 0) {
        virReportSystemError(errno,
                             _("Unable to pre-create chardev file '%s'"),
                             dev->source->data.file.path);
        return -1;
    }

    VIR_FORCE_CLOSE(fd);

    return 0;
}