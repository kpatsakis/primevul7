qemuProcessShutdownOrReboot(virQEMUDriverPtr driver,
                            virDomainObjPtr vm)
{
    qemuDomainObjPrivatePtr priv = vm->privateData;

    if (priv->fakeReboot) {
        g_autofree char *name = g_strdup_printf("reboot-%s", vm->def->name);
        virThread th;

        qemuDomainSetFakeReboot(driver, vm, false);
        virObjectRef(vm);
        if (virThreadCreateFull(&th,
                                false,
                                qemuProcessFakeReboot,
                                name,
                                false,
                                vm) < 0) {
            VIR_ERROR(_("Failed to create reboot thread, killing domain"));
            ignore_value(qemuProcessKill(vm, VIR_QEMU_PROCESS_KILL_NOWAIT));
            priv->pausedShutdown = false;
            virObjectUnref(vm);
        }
    } else {
        ignore_value(qemuProcessKill(vm, VIR_QEMU_PROCESS_KILL_NOWAIT));
    }
}