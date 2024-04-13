qemuProcessStartPRDaemonHook(void *opaque)
{
    virDomainObjPtr vm = opaque;
    size_t i, nfds = 0;
    g_autofree int *fds = NULL;
    int ret = -1;

    if (qemuDomainNamespaceEnabled(vm, QEMU_DOMAIN_NS_MOUNT)) {
        if (virProcessGetNamespaces(vm->pid, &nfds, &fds) < 0)
            return ret;

        if (nfds > 0 &&
            virProcessSetNamespaces(nfds, fds) < 0)
            goto cleanup;
    }

    ret = 0;
 cleanup:
    for (i = 0; i < nfds; i++)
        VIR_FORCE_CLOSE(fds[i]);
    return ret;
}