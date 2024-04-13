static void nvme_format_cancel(BlockAIOCB *aiocb)
{
    NvmeFormatAIOCB *iocb = container_of(aiocb, NvmeFormatAIOCB, common);

    if (iocb->aiocb) {
        blk_aio_cancel_async(iocb->aiocb);
    }
}