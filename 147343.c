qemuProcessStartValidateIOThreads(virDomainObjPtr vm,
                                  virQEMUCapsPtr qemuCaps)
{
    size_t i;

    if (vm->def->niothreadids > 0 &&
        !virQEMUCapsGet(qemuCaps, QEMU_CAPS_OBJECT_IOTHREAD)) {
        virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                       _("IOThreads not supported for this QEMU"));
        return -1;
    }

    for (i = 0; i < vm->def->ncontrollers; i++) {
        virDomainControllerDefPtr cont = vm->def->controllers[i];

        if (cont->type == VIR_DOMAIN_CONTROLLER_TYPE_SCSI &&
            cont->model == VIR_DOMAIN_CONTROLLER_MODEL_SCSI_VIRTIO_SCSI &&
            cont->iothread > 0 &&
            !virQEMUCapsGet(qemuCaps, QEMU_CAPS_VIRTIO_SCSI_IOTHREAD)) {
            virReportError(VIR_ERR_CONFIG_UNSUPPORTED, "%s",
                           _("IOThreads for virtio-scsi not supported for "
                             "this QEMU"));
            return -1;
        }
    }

    return 0;
}