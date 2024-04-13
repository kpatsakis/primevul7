lprn_bubble_gen(gx_device_printer * pdev, int x0, int x1, int y0, int y1)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;

    Bubble *bbl;
    int bx0, bx1, bx;

    assert(lprn->freeBubbleList != NULL);

    bbl = lprn->freeBubbleList;
    lprn->freeBubbleList = bbl->next;

    bbl->brect.p.x = x0;
    bbl->brect.q.x = x1;
    bbl->brect.p.y = y0;
    bbl->brect.q.y = y1;

    bx0 = x0 / lprn->nBw;
    bx1 = (x1 + lprn->nBw - 1) / lprn->nBw;

    for (bx = bx0; bx <= bx1; bx++) {
        assert(lprn->bubbleTbl[bx] == NULL);
        lprn->bubbleTbl[bx] = bbl;
    }
}