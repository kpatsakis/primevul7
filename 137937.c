static void nvme_copy_cancel(BlockAIOCB *aiocb)
{
    NvmeCopyAIOCB *iocb = container_of(aiocb, NvmeCopyAIOCB, common);

    iocb->ret = -ECANCELED;

    if (iocb->aiocb) {
        blk_aio_cancel_async(iocb->aiocb);
        iocb->aiocb = NULL;
    }
}