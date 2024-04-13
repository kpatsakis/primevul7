qemuProcessQMPStart(qemuProcessQMPPtr proc)
{
    VIR_DEBUG("proc=%p, emulator=%s", proc, proc->binary);

    if (qemuProcessQMPInit(proc) < 0)
        return -1;

    if (qemuProcessQMPLaunch(proc) < 0)
        return -1;

    if (qemuProcessQMPConnectMonitor(proc) < 0)
        return -1;

    return 0;
}