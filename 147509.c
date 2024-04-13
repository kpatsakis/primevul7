qemuProcessUpdateVideoRamSize(virQEMUDriverPtr driver,
                              virDomainObjPtr vm,
                              int asyncJob)
{
    int ret = -1;
    ssize_t i;
    qemuDomainObjPrivatePtr priv = vm->privateData;
    virDomainVideoDefPtr video = NULL;
    g_autoptr(virQEMUDriverConfig) cfg = NULL;

    if (qemuDomainObjEnterMonitorAsync(driver, vm, asyncJob) < 0)
        return -1;

    for (i = 0; i < vm->def->nvideos; i++) {
        video = vm->def->videos[i];

        switch (video->type) {
        case VIR_DOMAIN_VIDEO_TYPE_VGA:
            if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_VGA_VGAMEM)) {
                if (qemuMonitorUpdateVideoMemorySize(priv->mon, video, "VGA") < 0)
                    goto error;
            }
            break;
        case VIR_DOMAIN_VIDEO_TYPE_QXL:
            if (i == 0) {
                if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_QXL_VGAMEM) &&
                    qemuMonitorUpdateVideoMemorySize(priv->mon, video,
                                                     "qxl-vga") < 0)
                        goto error;

                if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_QXL_VRAM64) &&
                    qemuMonitorUpdateVideoVram64Size(priv->mon, video,
                                                     "qxl-vga") < 0)
                    goto error;
            } else {
                if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_QXL_VGAMEM) &&
                    qemuMonitorUpdateVideoMemorySize(priv->mon, video,
                                                     "qxl") < 0)
                        goto error;

                if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_QXL_VRAM64) &&
                    qemuMonitorUpdateVideoVram64Size(priv->mon, video,
                                                     "qxl") < 0)
                        goto error;
            }
            break;
        case VIR_DOMAIN_VIDEO_TYPE_VMVGA:
            if (virQEMUCapsGet(priv->qemuCaps, QEMU_CAPS_VMWARE_SVGA_VGAMEM)) {
                if (qemuMonitorUpdateVideoMemorySize(priv->mon, video,
                                                     "vmware-svga") < 0)
                    goto error;
            }
            break;
        case VIR_DOMAIN_VIDEO_TYPE_CIRRUS:
        case VIR_DOMAIN_VIDEO_TYPE_XEN:
        case VIR_DOMAIN_VIDEO_TYPE_VBOX:
        case VIR_DOMAIN_VIDEO_TYPE_LAST:
            break;
        }

    }

    if (qemuDomainObjExitMonitor(driver, vm) < 0)
        return -1;

    cfg = virQEMUDriverGetConfig(driver);
    ret = virDomainObjSave(vm, driver->xmlopt, cfg->stateDir);

    return ret;

 error:
    ignore_value(qemuDomainObjExitMonitor(driver, vm));
    return -1;
}