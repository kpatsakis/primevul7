qemuProcessMakeDir(virQEMUDriverPtr driver,
                   virDomainObjPtr vm,
                   const char *path)
{
    if (virFileMakePathWithMode(path, 0750) < 0) {
        virReportSystemError(errno, _("Cannot create directory '%s'"), path);
        return -1;
    }

    if (qemuSecurityDomainSetPathLabel(driver, vm, path, true) < 0)
        return -1;

    return 0;
}