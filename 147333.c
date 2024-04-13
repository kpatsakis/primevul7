qemuProcessQMPConnectMonitor(qemuProcessQMPPtr proc)
{
    virDomainXMLOptionPtr xmlopt = NULL;
    virDomainChrSourceDef monConfig;
    int ret = -1;

    VIR_DEBUG("proc=%p, emulator=%s, proc->pid=%lld",
              proc, proc->binary, (long long)proc->pid);

    monConfig.type = VIR_DOMAIN_CHR_TYPE_UNIX;
    monConfig.data.nix.path = proc->monpath;
    monConfig.data.nix.listen = false;

    if (!(xmlopt = virDomainXMLOptionNew(NULL, NULL, NULL, NULL, NULL)) ||
        !(proc->vm = virDomainObjNew(xmlopt)) ||
        !(proc->vm->def = virDomainDefNew()))
        goto cleanup;

    proc->vm->pid = proc->pid;

    if (!(proc->mon = qemuMonitorOpen(proc->vm, &monConfig, true, 0,
                                      virEventThreadGetContext(proc->eventThread),
                                      &callbacks, NULL)))
        goto cleanup;

    virObjectLock(proc->mon);

    if (qemuProcessQMPInitMonitor(proc->mon) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    virObjectUnref(xmlopt);
    return ret;
}