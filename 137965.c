static void nvme_irq_check(NvmeCtrl *n)
{
    uint32_t intms = ldl_le_p(&n->bar.intms);

    if (msix_enabled(&(n->parent_obj))) {
        return;
    }
    if (~intms & n->irq_status) {
        pci_irq_assert(&n->parent_obj);
    } else {
        pci_irq_deassert(&n->parent_obj);
    }
}