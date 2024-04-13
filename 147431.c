qemuProcessDestroyMemoryBackingPath(virQEMUDriverPtr driver,
                                    virDomainObjPtr vm,
                                    virDomainMemoryDefPtr mem)
{
    g_autofree char *path = NULL;

    if (qemuGetMemoryBackingPath(driver, vm->def, mem->info.alias, &path) < 0)
        return -1;

    if (unlink(path) < 0 &&
        errno != ENOENT) {
        virReportSystemError(errno, _("Unable to remove %s"), path);
        return -1;
    }

    return 0;
}