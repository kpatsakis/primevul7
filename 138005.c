static inline void nvme_blk_write(BlockBackend *blk, int64_t offset,
                                  BlockCompletionFunc *cb, NvmeRequest *req)
{
    assert(req->sg.flags & NVME_SG_ALLOC);

    if (req->sg.flags & NVME_SG_DMA) {
        req->aiocb = dma_blk_write(blk, &req->sg.qsg, offset, BDRV_SECTOR_SIZE,
                                   cb, req);
    } else {
        req->aiocb = blk_aio_pwritev(blk, offset, &req->sg.iov, 0, cb, req);
    }
}