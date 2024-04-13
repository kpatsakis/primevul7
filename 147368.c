qemuProcessStartValidateShmem(virDomainObjPtr vm)
{
    size_t i;

    for (i = 0; i < vm->def->nshmems; i++) {
        virDomainShmemDefPtr shmem = vm->def->shmems[i];

        if (strchr(shmem->name, '/')) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                           _("shmem name '%s' must not contain '/'"),
                           shmem->name);
            return -1;
        }
    }

    return 0;
}