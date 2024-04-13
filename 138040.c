static AioContext *nvme_get_aio_context(BlockAIOCB *acb)
{
    return qemu_get_aio_context();
}