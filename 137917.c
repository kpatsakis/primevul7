static void nvme_flush_cancel(BlockAIOCB *acb)
{
    NvmeFlushAIOCB *iocb = container_of(acb, NvmeFlushAIOCB, common);

    iocb->ret = -ECANCELED;

    if (iocb->aiocb) {
        blk_aio_cancel_async(iocb->aiocb);
    }
}