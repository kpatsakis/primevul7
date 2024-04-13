iscsi_schedule_bh(IscsiAIOCB *acb)
{
    if (acb->bh) {
        return;
    }
    acb->bh = aio_bh_new(acb->iscsilun->aio_context, iscsi_bh_cb, acb);
    qemu_bh_schedule(acb->bh);
}