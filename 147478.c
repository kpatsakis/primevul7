qemuProcessBeginStopJob(virQEMUDriverPtr driver,
                        virDomainObjPtr vm,
                        qemuDomainJob job,
                        bool forceKill)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;
    unsigned int killFlags = forceKill ? VIR_QEMU_PROCESS_KILL_FORCE : 0;
    int ret = -1;

    /* We need to prevent monitor EOF callback from doing our work (and
     * sending misleading events) while the vm is unlocked inside
     * BeginJob/ProcessKill API
     */
    priv->beingDestroyed = true;

    if (qemuProcessKill(vm, killFlags) < 0)
        goto cleanup;

    /* Wake up anything waiting on domain condition */
    virDomainObjBroadcast(vm);

    if (qemuDomainObjBeginJob(driver, vm, job) < 0)
        goto cleanup;

    ret = 0;

 cleanup:
    priv->beingDestroyed = false;
    return ret;
}