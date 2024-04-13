lprn_rect_add(gx_device_printer * pdev, gp_file * fp, int r, int h, int start, int end)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;

    int x0 = start * lprn->nBw;
    int x1 = end * lprn->nBw - 1;
    int y0 = r + h - lprn->nBh;
    int y1 = y0 + lprn->nBh - 1;
    int i;
    Bubble *bbl;

    if ((bbl = lprn->bubbleTbl[start]) != NULL &&
        bbl->brect.q.y == y0 - 1 &&
        bbl->brect.p.x == x0 &&
        bbl->brect.q.x == x1) {
        bbl->brect.q.y = y1;
    } else {
        for (i = start; i <= end; i++)
            if (lprn->bubbleTbl[i] != NULL)
                lprn_bubble_flush(pdev, fp, lprn->bubbleTbl[i]);
        lprn_bubble_gen(pdev, x0, x1, y0, y1);
    }
}