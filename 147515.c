qemuProcessIncomingDefNew(virQEMUCapsPtr qemuCaps,
                          const char *listenAddress,
                          const char *migrateFrom,
                          int fd,
                          const char *path)
{
    qemuProcessIncomingDefPtr inc = NULL;

    if (qemuMigrationDstCheckProtocol(qemuCaps, migrateFrom) < 0)
        return NULL;

    inc = g_new0(qemuProcessIncomingDef, 1);

    inc->address = g_strdup(listenAddress);

    inc->launchURI = qemuMigrationDstGetURI(migrateFrom, fd);
    if (!inc->launchURI)
        goto error;

    if (virQEMUCapsGet(qemuCaps, QEMU_CAPS_INCOMING_DEFER)) {
        inc->deferredURI = inc->launchURI;
        inc->launchURI = g_strdup("defer");
    }

    inc->fd = fd;
    inc->path = path;

    return inc;

 error:
    qemuProcessIncomingDefFree(inc);
    return NULL;
}