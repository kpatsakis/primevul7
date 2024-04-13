iscsi_co_writev(BlockDriverState *bs, int64_t sector_num, int nb_sectors,
                QEMUIOVector *iov, int flags)
{
    IscsiLun *iscsilun = bs->opaque;
    struct IscsiTask iTask;
    uint64_t lba;
    uint32_t num_sectors;
    bool fua = flags & BDRV_REQ_FUA;
    int r = 0;

    if (fua) {
        assert(iscsilun->dpofua);
    }
    if (!is_sector_request_lun_aligned(sector_num, nb_sectors, iscsilun)) {
        return -EINVAL;
    }

    if (bs->bl.max_transfer) {
        assert(nb_sectors << BDRV_SECTOR_BITS <= bs->bl.max_transfer);
    }

    lba = sector_qemu2lun(sector_num, iscsilun);
    num_sectors = sector_qemu2lun(nb_sectors, iscsilun);
    iscsi_co_init_iscsitask(iscsilun, &iTask);
    qemu_mutex_lock(&iscsilun->mutex);
retry:
    if (iscsilun->use_16_for_rw) {
#if LIBISCSI_API_VERSION >= (20160603)
        iTask.task = iscsi_write16_iov_task(iscsilun->iscsi, iscsilun->lun, lba,
                                            NULL, num_sectors * iscsilun->block_size,
                                            iscsilun->block_size, 0, 0, fua, 0, 0,
                                            iscsi_co_generic_cb, &iTask,
                                            (struct scsi_iovec *)iov->iov, iov->niov);
    } else {
        iTask.task = iscsi_write10_iov_task(iscsilun->iscsi, iscsilun->lun, lba,
                                            NULL, num_sectors * iscsilun->block_size,
                                            iscsilun->block_size, 0, 0, fua, 0, 0,
                                            iscsi_co_generic_cb, &iTask,
                                            (struct scsi_iovec *)iov->iov, iov->niov);
    }
#else
        iTask.task = iscsi_write16_task(iscsilun->iscsi, iscsilun->lun, lba,
                                        NULL, num_sectors * iscsilun->block_size,
                                        iscsilun->block_size, 0, 0, fua, 0, 0,
                                        iscsi_co_generic_cb, &iTask);
    } else {