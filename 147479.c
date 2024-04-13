qemuProcessBuildPRHelperPidfilePath(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    const char *prdAlias = qemuDomainGetManagedPRAlias();

    return virPidFileBuildPath(priv->libDir, prdAlias);
}