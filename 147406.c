qemuProcessSetupIOThread(virDomainObjPtr vm,
                         virDomainIOThreadIDDefPtr iothread)
{
    return qemuProcessSetupPid(vm, iothread->thread_id,
                               VIR_CGROUP_THREAD_IOTHREAD,
                               iothread->iothread_id,
                               iothread->cpumask,
                               vm->def->cputune.iothread_period,
                               vm->def->cputune.iothread_quota,
                               &iothread->sched);
}