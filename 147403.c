qemuLogOperation(virDomainObjPtr vm,
                 const char *msg,
                 virCommandPtr cmd,
                 qemuDomainLogContextPtr logCtxt)
{
    g_autofree char *timestamp = NULL;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    int qemuVersion = virQEMUCapsGetVersion(priv->qemuCaps);
    const char *package = virQEMUCapsGetPackage(priv->qemuCaps);
    g_autofree char *hostname = virGetHostname();
    struct utsname uts;

    uname(&uts);

    if ((timestamp = virTimeStringNow()) == NULL)
        return;

    if (qemuDomainLogContextWrite(logCtxt,
                                  "%s: %s %s, qemu version: %d.%d.%d%s, kernel: %s, hostname: %s\n",
                                  timestamp, msg, VIR_LOG_VERSION_STRING,
                                  (qemuVersion / 1000000) % 1000,
                                  (qemuVersion / 1000) % 1000,
                                  qemuVersion % 1000,
                                  NULLSTR_EMPTY(package),
                                  uts.release,
                                  NULLSTR_EMPTY(hostname)) < 0)
        return;

    if (cmd) {
        g_autofree char *args = virCommandToString(cmd, true);
        qemuDomainLogContextWrite(logCtxt, "%s\n", args);
    }
}