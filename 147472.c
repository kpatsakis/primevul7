qemuRefreshRTC(virQEMUDriverPtr driver,
               virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    time_t now, then;
    struct tm thenbits;
    long localOffset;
    int rv;

    if (vm->def->clock.offset != VIR_DOMAIN_CLOCK_OFFSET_VARIABLE)
        return;

    memset(&thenbits, 0, sizeof(thenbits));
    qemuDomainObjEnterMonitor(driver, vm);
    now = time(NULL);
    rv = qemuMonitorGetRTCTime(priv->mon, &thenbits);
    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        rv = -1;

    if (rv < 0)
        return;

    thenbits.tm_isdst = -1;
    if ((then = mktime(&thenbits)) == (time_t)-1) {
        virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                       _("Unable to convert time"));
        return;
    }

    /* Thing is, @now is in local TZ but @then in UTC. */
    if (virTimeLocalOffsetFromUTC(&localOffset) < 0)
        return;

    vm->def->clock.data.variable.adjustment = then - now + localOffset;
}