qemuProcessSetupHotpluggableVcpus(virQEMUDriverPtr driver,
                                  virDomainObjPtr vm,
                                  qemuDomainAsyncJob asyncJob)
{
    unsigned int maxvcpus = virDomainDefGetVcpusMax(vm->def);
    qemuDomainObjPrivatePtr priv = vm->privateData;
    qemuCgroupEmulatorAllNodesDataPtr emulatorCgroup = NULL;
    virDomainVcpuDefPtr vcpu;
    qemuDomainVcpuPrivatePtr vcpupriv;
    virJSONValuePtr vcpuprops = NULL;
    size_t i;
    int ret = -1;
    int rc;

    virDomainVcpuDefPtr *bootHotplug = NULL;
    size_t nbootHotplug = 0;

    for (i = 0; i < maxvcpus; i++) {
        vcpu = virDomainDefGetVcpu(vm->def, i);
        vcpupriv = QEMU_DOMAIN_VCPU_PRIVATE(vcpu);

        if (vcpu->hotpluggable == VIR_TRISTATE_BOOL_YES && vcpu->online &&
            vcpupriv->vcpus != 0) {
            vcpupriv->alias = g_strdup_printf("vcpu%zu", i);

            if (VIR_APPEND_ELEMENT(bootHotplug, nbootHotplug, vcpu) < 0)
                goto cleanup;
        }
    }

    if (nbootHotplug == 0) {
        ret = 0;
        goto cleanup;
    }

    qsort(bootHotplug, nbootHotplug, sizeof(*bootHotplug),
          qemuProcessVcpusSortOrder);

    if (qemuCgroupEmulatorAllNodesAllow(priv->cgroup, &emulatorCgroup) < 0)
        goto cleanup;

    for (i = 0; i < nbootHotplug; i++) {
        vcpu = bootHotplug[i];

        if (!(vcpuprops = qemuBuildHotpluggableCPUProps(vcpu)))
            goto cleanup;

        if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
            goto cleanup;

        rc = qemuMonitorAddDeviceArgs(qemuDomainGetMonitor(vm), vcpuprops);
        vcpuprops = NULL;

        if (qemuDomainObjExitMonitor(driver, vm) < 0)
            goto cleanup;

        if (rc < 0)
            goto cleanup;

        virJSONValueFree(vcpuprops);
    }

    ret = 0;

 cleanup:
    qemuCgroupEmulatorAllNodesRestore(emulatorCgroup);
    VIR_FREE(bootHotplug);
    virJSONValueFree(vcpuprops);
    return ret;
}