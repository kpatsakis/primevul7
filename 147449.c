qemuProcessGenID(virDomainObjPtr vm,
                 unsigned int flags)
{
    if (!vm->def->genidRequested)
        return 0;

    /* If we are coming from a path where we must provide a new gen id
     * value regardless of whether it was previously generated or provided,
     * then generate a new GUID value before we build the command line. */
    if (flags & VIR_QEMU_PROCESS_START_GEN_VMID) {
        if (virUUIDGenerate(vm->def->genid) < 0) {
            virReportError(VIR_ERR_INTERNAL_ERROR, "%s",
                           _("failed to regenerate genid"));
            return -1;
        }
    }

    return 0;
}