static bool nvme_nsid_valid(NvmeCtrl *n, uint32_t nsid)
{
    return nsid &&
        (nsid == NVME_NSID_BROADCAST || nsid <= NVME_MAX_NAMESPACES);
}