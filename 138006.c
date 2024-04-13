static void nvme_free_cq(NvmeCQueue *cq, NvmeCtrl *n)
{
    n->cq[cq->cqid] = NULL;
    timer_free(cq->timer);
    if (msix_enabled(&n->parent_obj)) {
        msix_vector_unuse(&n->parent_obj, cq->vector);
    }
    if (cq->cqid) {
        g_free(cq);
    }
}