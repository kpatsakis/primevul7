lprn_process_line(gx_device_printer * pdev, gp_file * fp, int r, int h)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;

    int bx, bInBlack, bBlack, start;
    int bpl = gdev_mem_bytes_per_scan_line(pdev);
    int maxBx = (bpl + lprn->nBw - 1) / lprn->nBw;

    bInBlack = 0;
    for (bx = 0; bx < maxBx; bx++) {
        bBlack = lprn_is_black(pdev, r, h, bx);
        if (!bInBlack) {
            if (bBlack) {
                start = bx;
                bInBlack = 1;
            }
        } else {
            if (!bBlack) {
                bInBlack = 0;
                lprn_rect_add(pdev, fp, r, h, start, bx);
            }
        }
    }
    if (bInBlack)
        lprn_rect_add(pdev, fp, r, h, start, bx - 1);
}