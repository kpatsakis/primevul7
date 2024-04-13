lprn_bubble_flush(gx_device_printer * pdev, gp_file * fp, Bubble * bbl)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;
    int i, j, bx;
    byte *p;
    int bx0 = bbl->brect.p.x / lprn->nBw;
    int bx1 = (bbl->brect.q.x + lprn->nBw - 1) / lprn->nBw;
    int bpl = gdev_mem_bytes_per_scan_line(pdev);
    int x = bbl->brect.p.x * 8;
    int y = bbl->brect.p.y;
    int width = (bbl->brect.q.x - bbl->brect.p.x + 1) * 8;
    int height = bbl->brect.q.y - bbl->brect.p.y + 1;
    int maxY = lprn->BlockLine / lprn->nBh * lprn->nBh;

    for (i = 0; i < height; i++) {
        p = lprn->ImageBuf + ((i + y) % maxY) * bpl;
        for (j = 0; j < width / 8; j++) {
            if (lprn->NegativePrint)
                *(lprn->TmpBuf + i * width / 8 + j) = ~*(p + j + bbl->brect.p.x);
            else
                *(lprn->TmpBuf + i * width / 8 + j) = *(p + j + bbl->brect.p.x);
        }
    }

    (*lprn->image_out) (pdev, fp, x, y, width, height);

    /* Initialize bubbleTbl */
    for (bx = bx0; bx <= bx1; bx++) {
        assert(lprn->bubbleTbl[bx] == bbl);
        lprn->bubbleTbl[bx] = NULL;
    }

    bbl->next = lprn->freeBubbleList;
    lprn->freeBubbleList = bbl;
}