static void nvme_irq_deassert(NvmeCtrl *n, NvmeCQueue *cq)
{
    if (cq->irq_enabled) {
        if (msix_enabled(&(n->parent_obj))) {
            return;
        } else {
            assert(cq->vector < 32);
            if (!n->cq_pending) {
                n->irq_status &= ~(1 << cq->vector);
            }
            nvme_irq_check(n);
        }
    }
}