static void nvme_check_constraints(NvmeCtrl *n, Error **errp)
{
    NvmeParams *params = &n->params;

    if (params->num_queues) {
        warn_report("num_queues is deprecated; please use max_ioqpairs "
                    "instead");

        params->max_ioqpairs = params->num_queues - 1;
    }

    if (n->namespace.blkconf.blk && n->subsys) {
        error_setg(errp, "subsystem support is unavailable with legacy "
                   "namespace ('drive' property)");
        return;
    }

    if (params->max_ioqpairs < 1 ||
        params->max_ioqpairs > NVME_MAX_IOQPAIRS) {
        error_setg(errp, "max_ioqpairs must be between 1 and %d",
                   NVME_MAX_IOQPAIRS);
        return;
    }

    if (params->msix_qsize < 1 ||
        params->msix_qsize > PCI_MSIX_FLAGS_QSIZE + 1) {
        error_setg(errp, "msix_qsize must be between 1 and %d",
                   PCI_MSIX_FLAGS_QSIZE + 1);
        return;
    }

    if (!params->serial) {
        error_setg(errp, "serial property not set");
        return;
    }

    if (n->pmr.dev) {
        if (host_memory_backend_is_mapped(n->pmr.dev)) {
            error_setg(errp, "can't use already busy memdev: %s",
                       object_get_canonical_path_component(OBJECT(n->pmr.dev)));
            return;
        }

        if (!is_power_of_2(n->pmr.dev->size)) {
            error_setg(errp, "pmr backend size needs to be power of 2 in size");
            return;
        }

        host_memory_backend_set_mapped(n->pmr.dev, true);
    }

    if (n->params.zasl > n->params.mdts) {
        error_setg(errp, "zoned.zasl (Zone Append Size Limit) must be less "
                   "than or equal to mdts (Maximum Data Transfer Size)");
        return;
    }

    if (!n->params.vsl) {
        error_setg(errp, "vsl must be non-zero");
        return;
    }
}