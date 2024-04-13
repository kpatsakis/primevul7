static void nvme_update_dmrsl(NvmeCtrl *n)
{
    int nsid;

    for (nsid = 1; nsid <= NVME_MAX_NAMESPACES; nsid++) {
        NvmeNamespace *ns = nvme_ns(n, nsid);
        if (!ns) {
            continue;
        }

        n->dmrsl = MIN_NON_ZERO(n->dmrsl,
                                BDRV_REQUEST_MAX_BYTES / nvme_l2b(ns, 1));
    }
}