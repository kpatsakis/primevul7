qemuProcessSetupGraphics(virQEMUDriverPtr driver,
                         virDomainObjPtr vm,
                         virQEMUCapsPtr qemuCaps,
                         unsigned int flags)
{
    virDomainGraphicsDefPtr graphics;
    bool allocate = !(flags & VIR_QEMU_PROCESS_START_PRETEND);
    size_t i;

    for (i = 0; i < vm->def->ngraphics; i++) {
        graphics = vm->def->graphics[i];

        if (qemuProcessGraphicsSetupRenderNode(graphics, qemuCaps) < 0)
            return -1;

        if (qemuProcessGraphicsSetupListen(driver, graphics, vm) < 0)
            return -1;
    }

    if (allocate) {
        for (i = 0; i < vm->def->ngraphics; i++) {
            graphics = vm->def->graphics[i];

            if (qemuProcessGraphicsReservePorts(graphics, false) < 0)
                return -1;
        }
    }

    for (i = 0; i < vm->def->ngraphics; ++i) {
        graphics = vm->def->graphics[i];

        if (qemuProcessGraphicsAllocatePorts(driver, graphics, allocate) < 0)
            return -1;
    }

    return 0;
}