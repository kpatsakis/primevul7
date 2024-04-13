static int nvme_addr_write(NvmeCtrl *n, hwaddr addr, void *buf, int size)
{
    hwaddr hi = addr + size - 1;
    if (hi < addr) {
        return 1;
    }

    if (n->bar.cmbsz && nvme_addr_is_cmb(n, addr) && nvme_addr_is_cmb(n, hi)) {
        memcpy(nvme_addr_to_cmb(n, addr), buf, size);
        return 0;
    }

    if (nvme_addr_is_pmr(n, addr) && nvme_addr_is_pmr(n, hi)) {
        memcpy(nvme_addr_to_pmr(n, addr), buf, size);
        return 0;
    }

    return pci_dma_write(&n->parent_obj, addr, buf, size);
}