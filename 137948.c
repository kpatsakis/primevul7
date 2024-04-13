static void nvme_post_cqes(void *opaque)
{
    NvmeCQueue *cq = opaque;
    NvmeCtrl *n = cq->ctrl;
    NvmeRequest *req, *next;
    bool pending = cq->head != cq->tail;
    int ret;

    QTAILQ_FOREACH_SAFE(req, &cq->req_list, entry, next) {
        NvmeSQueue *sq;
        hwaddr addr;

        if (nvme_cq_full(cq)) {
            break;
        }

        sq = req->sq;
        req->cqe.status = cpu_to_le16((req->status << 1) | cq->phase);
        req->cqe.sq_id = cpu_to_le16(sq->sqid);
        req->cqe.sq_head = cpu_to_le16(sq->head);
        addr = cq->dma_addr + cq->tail * n->cqe_size;
        ret = pci_dma_write(&n->parent_obj, addr, (void *)&req->cqe,
                            sizeof(req->cqe));
        if (ret) {
            trace_pci_nvme_err_addr_write(addr);
            trace_pci_nvme_err_cfs();
            stl_le_p(&n->bar.csts, NVME_CSTS_FAILED);
            break;
        }
        QTAILQ_REMOVE(&cq->req_list, req, entry);
        nvme_inc_cq_tail(cq);
        nvme_sg_unmap(&req->sg);
        QTAILQ_INSERT_TAIL(&sq->req_list, req, entry);
    }
    if (cq->tail != cq->head) {
        if (cq->irq_enabled && !pending) {
            n->cq_pending++;
        }

        nvme_irq_assert(n, cq);
    }
}