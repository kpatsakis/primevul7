qemuProcessBuildDestroyMemoryPathsImpl(virQEMUDriverPtr driver,
                                       virDomainObjPtr vm,
                                       const char *path,
                                       bool build)
{
    if (build) {
        if (virFileExists(path))
            return 0;

        if (virFileMakePathWithMode(path, 0700) < 0) {
            virReportSystemError(errno,
                                 _("Unable to create %s"),
                                 path);
            return -1;
        }

        if (qemuSecurityDomainSetPathLabel(driver, vm, path, true) < 0)
            return -1;
    } else {
        if (virFileDeleteTree(path) < 0)
            return -1;
    }

    return 0;
}