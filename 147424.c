qemuProcessEnablePerf(virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    size_t i;

    if (!(priv->perf = virPerfNew()))
        return -1;

    for (i = 0; i < VIR_PERF_EVENT_LAST; i++) {
        if (vm->def->perf.events[i] == VIR_TRISTATE_BOOL_YES &&
            virPerfEventEnable(priv->perf, i, vm->pid) < 0)
            return -1;
    }

    return 0;
}