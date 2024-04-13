qemuProcessBuildDestroyMemoryPaths(virQEMUDriverPtr driver,
                                   virDomainObjPtr vm,
                                   virDomainMemoryDefPtr mem,
                                   bool build)
{

    g_autoptr(virQEMUDriverConfig) cfg = virQEMUDriverGetConfig(driver);
    size_t i;
    bool shouldBuildHP = false;
    bool shouldBuildMB = false;

    if (build) {
        shouldBuildHP = qemuProcessNeedHugepagesPath(vm->def, mem);
        shouldBuildMB = qemuProcessNeedMemoryBackingPath(vm->def, mem);
    }

    if (!build || shouldBuildHP) {
        for (i = 0; i < cfg->nhugetlbfs; i++) {
            g_autofree char *path = NULL;
            path = qemuGetDomainHugepagePath(driver, vm->def, &cfg->hugetlbfs[i]);

            if (!path)
                return -1;

            if (qemuProcessBuildDestroyMemoryPathsImpl(driver, vm,
                                                       path, build) < 0)
                return -1;
        }
    }

    if (!build || shouldBuildMB) {
        g_autofree char *path = NULL;
        if (qemuGetMemoryBackingDomainPath(driver, vm->def, &path) < 0)
            return -1;

        if (qemuProcessBuildDestroyMemoryPathsImpl(driver, vm,
                                                   path, build) < 0)
            return -1;
    }

    return 0;
}