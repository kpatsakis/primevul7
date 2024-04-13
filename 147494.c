qemuProcessQMPInitMonitor(qemuMonitorPtr mon)
{
    if (qemuMonitorSetCapabilities(mon) < 0) {
        VIR_DEBUG("Failed to set monitor capabilities %s",
                  virGetLastErrorMessage());
        return -1;
    }

    return 0;
}