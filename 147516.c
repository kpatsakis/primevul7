qemuDomainPerfRestart(virDomainObjPtr vm)
{
    size_t i;
    virDomainDefPtr def = vm->def;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (!(priv->perf = virPerfNew()))
        return -1;

    for (i = 0; i < VIR_PERF_EVENT_LAST; i++) {
        if (def->perf.events[i] &&
            def->perf.events[i] == VIR_TRISTATE_BOOL_YES) {

            /* Failure to re-enable the perf event should not be fatal */
            if (virPerfEventEnable(priv->perf, i, vm->pid) < 0)
                def->perf.events[i] = VIR_TRISTATE_BOOL_NO;
        }
    }

    return 0;
}