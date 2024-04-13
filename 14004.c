lprn_bubble_flush_all(gx_device_printer * pdev, gp_file * fp)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;
    int i = 0;
    int bpl = gdev_mem_bytes_per_scan_line(pdev);
    int maxBx = (bpl + lprn->nBw - 1) / lprn->nBw;

    for (i = 0; i < maxBx; i++) {
        if (lprn->bubbleTbl[i] != NULL) {
            lprn_bubble_flush(pdev, fp, lprn->bubbleTbl[i]);
        } else
            break;
    }
}