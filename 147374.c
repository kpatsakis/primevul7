qemuProcessQMPStop(qemuProcessQMPPtr proc)
{
    if (proc->mon) {
        virObjectUnlock(proc->mon);
        qemuMonitorClose(proc->mon);
        proc->mon = NULL;
    }

    if (proc->cmd) {
        virCommandAbort(proc->cmd);
        virCommandFree(proc->cmd);
        proc->cmd = NULL;
    }

    if (proc->monpath)
        unlink(proc->monpath);

    virDomainObjEndAPI(&proc->vm);

    if (proc->pid != 0) {
        VIR_DEBUG("Killing QMP caps process %lld", (long long)proc->pid);
        if (virProcessKill(proc->pid, SIGKILL) < 0 && errno != ESRCH)
            VIR_ERROR(_("Failed to kill process %lld: %s"),
                      (long long)proc->pid,
                      g_strerror(errno));

        proc->pid = 0;
    }

    if (proc->pidfile)
        unlink(proc->pidfile);

    if (proc->uniqDir)
        rmdir(proc->uniqDir);
}