qemuProcessStartValidateTSC(virQEMUDriverPtr driver,
                            virDomainObjPtr vm)
{
    size_t i;
    unsigned long long freq = 0;
    unsigned long long tolerance;
    unsigned long long minFreq;
    unsigned long long maxFreq;
    virHostCPUTscInfoPtr tsc;
    g_autoptr(virCPUDef) cpu = NULL;

    for (i = 0; i < vm->def->clock.ntimers; i++) {
        virDomainTimerDefPtr timer = vm->def->clock.timers[i];

        if (timer->name == VIR_DOMAIN_TIMER_NAME_TSC &&
            timer->frequency > 0) {
            freq = timer->frequency;
            break;
        }
    }

    if (freq == 0)
        return 0;

    VIR_DEBUG("Requested TSC frequency %llu Hz", freq);

    cpu = virQEMUDriverGetHostCPU(driver);
    if (!cpu || !cpu->tsc) {
        VIR_DEBUG("Host TSC frequency could not be probed");
        return 0;
    }

    tsc = cpu->tsc;
    tolerance = tsc->frequency * TSC_TOLERANCE / 1000000;
    minFreq = tsc->frequency - tolerance;
    maxFreq = tsc->frequency + tolerance;

    VIR_DEBUG("Host TSC frequency %llu Hz, scaling %s, tolerance +/- %llu Hz",
              tsc->frequency, virTristateBoolTypeToString(tsc->scaling),
              tolerance);

    if (freq >= minFreq && freq <= maxFreq) {
        VIR_DEBUG("Requested TSC frequency is within tolerance interval");
        return 0;
    }

    if (tsc->scaling == VIR_TRISTATE_BOOL_YES)
        return 0;

    if (tsc->scaling == VIR_TRISTATE_BOOL_ABSENT) {
        VIR_DEBUG("Requested TSC frequency falls outside tolerance range and "
                  "scaling support is unknown, QEMU will try and possibly "
                  "fail later");
        return 0;
    }

    virReportError(VIR_ERR_CONFIG_UNSUPPORTED,
                   _("Requested TSC frequency %llu Hz is outside tolerance "
                     "range ([%llu, %llu] Hz) around host frequency %llu Hz "
                     "and TSC scaling is not supported by the host CPU"),
                   freq, minFreq, maxFreq, tsc->frequency);
    return -1;
}