qemuProcessQMPNew(const char *binary,
                  const char *libDir,
                  uid_t runUid,
                  gid_t runGid,
                  bool forceTCG)
{
    g_autoptr(qemuProcessQMP) proc = NULL;
    const char *threadSuffix;
    g_autofree char *threadName = NULL;

    VIR_DEBUG("exec=%s, libDir=%s, runUid=%u, runGid=%u, forceTCG=%d",
              binary, libDir, runUid, runGid, forceTCG);

    proc = g_new0(qemuProcessQMP, 1);

    proc->binary = g_strdup(binary);
    proc->libDir = g_strdup(libDir);

    proc->runUid = runUid;
    proc->runGid = runGid;
    proc->forceTCG = forceTCG;

    threadSuffix = strrchr(binary, '-');
    if (threadSuffix)
        threadSuffix++;
    else
        threadSuffix = binary;
    threadName = g_strdup_printf("qmp-%s", threadSuffix);

    if (!(proc->eventThread = virEventThreadNew(threadName)))
        return NULL;

    return g_steal_pointer(&proc);
}