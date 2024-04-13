void nvme_attach_ns(NvmeCtrl *n, NvmeNamespace *ns)
{
    uint32_t nsid = ns->params.nsid;
    assert(nsid && nsid <= NVME_MAX_NAMESPACES);

    n->namespaces[nsid] = ns;
    ns->attached++;

    n->dmrsl = MIN_NON_ZERO(n->dmrsl,
                            BDRV_REQUEST_MAX_BYTES / nvme_l2b(ns, 1));
}