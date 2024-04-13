qemuProcessSEVCreateFile(virDomainObjPtr vm,
                         const char *name,
                         const char *data)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virQEMUDriverPtr driver = priv->driver;
    g_autofree char *configFile = NULL;

    if (!(configFile = virFileBuildPath(priv->libDir, name, ".base64")))
        return -1;

    if (virFileRewriteStr(configFile, S_IRUSR | S_IWUSR, data) < 0) {
        virReportSystemError(errno, _("failed to write data to config '%s'"),
                             configFile);
        return -1;
    }

    if (qemuSecurityDomainSetPathLabel(driver, vm, configFile, true) < 0)
        return -1;

    return 0;
}