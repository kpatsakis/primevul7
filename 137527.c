static void ide_sector_read_cb(void *opaque, int ret)
{
    IDEState *s = opaque;
    int n;

    s->pio_aiocb = NULL;
    s->status &= ~BUSY_STAT;

    bdrv_acct_done(s->bs, &s->acct);
    if (ret != 0) {
        if (ide_handle_rw_error(s, -ret, BM_STATUS_PIO_RETRY |
                                BM_STATUS_RETRY_READ)) {
            return;
        }
    }

    n = s->nsector;
    if (n > s->req_nb_sectors) {
        n = s->req_nb_sectors;
    }

    /* Allow the guest to read the io_buffer */
    ide_transfer_start(s, s->io_buffer, n * BDRV_SECTOR_SIZE, ide_sector_read);

    ide_set_irq(s->bus);

    ide_set_sector(s, ide_get_sector(s) + n);
    s->nsector -= n;
}