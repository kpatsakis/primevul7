qemuProcessQMPFree(qemuProcessQMPPtr proc)
{
    if (!proc)
        return;

    qemuProcessQMPStop(proc);

    g_object_unref(proc->eventThread);

    g_free(proc->binary);
    g_free(proc->libDir);
    g_free(proc->uniqDir);
    g_free(proc->monpath);
    g_free(proc->monarg);
    g_free(proc->pidfile);
    g_free(proc->stdErr);
    g_free(proc);
}