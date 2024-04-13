qemuProcessFindDomainDiskByAliasOrQOM(virDomainObjPtr vm,
                                      const char *alias,
                                      const char *qomid)
{
    size_t i;

    if (alias && *alias == '\0')
        alias = NULL;

    if (alias)
        alias = qemuAliasDiskDriveSkipPrefix(alias);

    for (i = 0; i < vm->def->ndisks; i++) {
        virDomainDiskDefPtr disk = vm->def->disks[i];
        qemuDomainDiskPrivatePtr diskPriv = QEMU_DOMAIN_DISK_PRIVATE(disk);

        if ((disk->info.alias && STREQ_NULLABLE(disk->info.alias, alias)) ||
            (diskPriv->qomName && STREQ_NULLABLE(diskPriv->qomName, qomid)))
            return disk;
    }

    virReportError(VIR_ERR_INTERNAL_ERROR,
                   _("no disk found with alias '%s' or id '%s'"),
                   NULLSTR(alias), NULLSTR(qomid));
    return NULL;
}