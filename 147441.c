qemuProcessPrepareSEVGuestInput(virDomainObjPtr vm)
{
    virDomainSEVDefPtr sev = vm->def->sev;

    if (!sev)
        return 0;

    VIR_DEBUG("Preparing SEV guest");

    if (sev->dh_cert) {
        if (qemuProcessSEVCreateFile(vm, "dh_cert", sev->dh_cert) < 0)
            return -1;
    }

    if (sev->session) {
        if (qemuProcessSEVCreateFile(vm, "session", sev->session) < 0)
            return -1;
    }

    return 0;
}