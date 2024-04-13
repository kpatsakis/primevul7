qemuProcessStartValidateDisks(virDomainObjPtr vm,
                              virQEMUCapsPtr qemuCaps)
{
    size_t i;

    for (i = 0; i < vm->def->ndisks; i++) {
        virDomainDiskDefPtr disk = vm->def->disks[i];
        virStorageSourcePtr src = disk->src;

        /* This is a best effort check as we can only check if the command
         * option exists, but we cannot determine whether the running QEMU
         * was build with '--enable-vxhs'. */
        if (src->type == VIR_STORAGE_TYPE_NETWORK &&
            src->protocol == VIR_STORAGE_NET_PROTOCOL_VXHS &&
            !virQEMUCapsGet(qemuCaps, QEMU_CAPS_VXHS)) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                           _("VxHS protocol is not supported with this "
                             "QEMU binary"));
            return -1;
        }

        /* PowerPC pseries based VMs do not support floppy device */
        if (disk->device == VIR_DOMAIN_DISK_DEVICE_FLOPPY &&
            qemuDomainIsPSeries(vm->def)) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                           _("PowerPC pseries machines do not support floppy device"));
            return -1;
        }

        if (src->type == VIR_STORAGE_TYPE_NVME &&
            !virQEMUCapsGet(qemuCaps, QEMU_CAPS_DRIVE_NVME)) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                           _("NVMe disks are not supported with this QEMU binary"));
            return -1;
        }
    }

    return 0;
}