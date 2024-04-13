qemuProcessRefreshLegacyBlockjob(void *payload,
                                 const char *name,
                                 void *opaque)
{
    const char *jobname = name;
    virDomainObjPtr vm = opaque;
    qemuMonitorBlockJobInfoPtr info = payload;
    virDomainDiskDefPtr disk;
    qemuBlockJobDataPtr job;
    qemuBlockJobType jobtype = info->type;
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (!(disk = qemuProcessFindDomainDiskByAliasOrQOM(vm, jobname, jobname))) {
        VIR_DEBUG("could not find disk for block job '%s'", jobname);
        return 0;
    }

    if (jobtype == QEMU_BLOCKJOB_TYPE_COMMIT &&
        disk->mirrorJob == VIR_DOMAIN_BLOCK_JOB_TYPE_ACTIVE_COMMIT)
        jobtype = disk->mirrorJob;

    if (!(job = qemuBlockJobDiskNew(vm, disk, jobtype, jobname)))
        return -1;

    if (disk->mirror) {
        if ((!info->ready_present && info->end == info->cur) ||
            info->ready) {
            disk->mirrorState = VIR_DOMAIN_DISK_MIRROR_STATE_READY;
            job->state = VIR_DOMAIN_BLOCK_JOB_READY;
        }

        /* Pre-blockdev block copy labelled the chain of the mirrored device
         * just before pivoting. At that point it was no longer known whether
         * it's even necessary (e.g. disk is being reused). This code fixes
         * the labelling in case the job was started in a libvirt version
         * which did not label the chain when the block copy is being started.
         * Note that we can't do much on failure. */
        if (disk->mirrorJob == VIR_DOMAIN_BLOCK_JOB_TYPE_COPY) {
            if (qemuDomainDetermineDiskChain(priv->driver, vm, disk,
                                             disk->mirror, true) < 0)
                goto cleanup;

            if (disk->mirror->format &&
                disk->mirror->format != VIR_STORAGE_FILE_RAW &&
                (qemuDomainNamespaceSetupDisk(vm, disk->mirror) < 0 ||
                 qemuSetupImageChainCgroup(vm, disk->mirror) < 0 ||
                 qemuSecuritySetImageLabel(priv->driver, vm, disk->mirror,
                                           true, true) < 0))
                goto cleanup;
        }
    }

    qemuBlockJobStarted(job, vm);

 cleanup:
    qemuBlockJobStartupFinalize(vm, job);

    return 0;
}