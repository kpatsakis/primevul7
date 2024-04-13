qemuProcessTranslateCPUFeatures(const char *name,
                                void *opaque)
{
    virQEMUCapsPtr qemuCaps = opaque;

    return virQEMUCapsCPUFeatureFromQEMU(qemuCaps, name);
}