qemuProcessCleanupChardevDevice(virDomainDefPtr def G_GNUC_UNUSED,
                                virDomainChrDefPtr dev,
                                void *opaque G_GNUC_UNUSED)
{
    if (dev->source->type == VIR_DOMAIN_CHR_TYPE_UNIX &&
        dev->source->data.nix.listen &&
        dev->source->data.nix.path)
        unlink(dev->source->data.nix.path);

    return 0;
}