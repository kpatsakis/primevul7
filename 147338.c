qemuProcessHandleAcpiOstInfo(qemuMonitorPtr mon G_GNUC_UNUSED,
                             virDomainObjPtr vm,
                             const char *alias,
                             const char *slotType,
                             const char *slot,
                             unsigned int source,
                             unsigned int status,
                             void *opaque)
{
    virQEMUDriverPtr driver = opaque;
    virObjectEventPtr event = NULL;

    virObjectLock(vm);

    VIR_DEBUG("ACPI OST info for device %s domain %p %s. "
              "slotType='%s' slot='%s' source=%u status=%u",
              NULLSTR(alias), vm, vm->def->name, slotType, slot, source, status);

    if (!alias)
        goto cleanup;

    if (STREQ(slotType, "DIMM")) {
        if ((source == 0x003 || source == 0x103) &&
            (status == 0x01 || (status >= 0x80 && status <= 0x83))) {
            qemuDomainSignalDeviceRemoval(vm, alias,
                                          QEMU_DOMAIN_UNPLUGGING_DEVICE_STATUS_GUEST_REJECTED);

            event = virDomainEventDeviceRemovalFailedNewFromObj(vm, alias);
        }
    }

 cleanup:
    virObjectUnlock(vm);
    virObjectEventStateQueue(driver->domainEventState, event);

    return 0;
}