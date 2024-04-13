static void nvme_dsm_cancel(BlockAIOCB *aiocb)
{
    NvmeDSMAIOCB *iocb = container_of(aiocb, NvmeDSMAIOCB, common);

    /* break nvme_dsm_cb loop */
    iocb->idx = iocb->nr;
    iocb->ret = -ECANCELED;

    if (iocb->aiocb) {
        blk_aio_cancel_async(iocb->aiocb);
        iocb->aiocb = NULL;
    } else {
        /*
         * We only reach this if nvme_dsm_cancel() has already been called or
         * the command ran to completion and nvme_dsm_bh is scheduled to run.
         */
        assert(iocb->idx == iocb->nr);
    }
}