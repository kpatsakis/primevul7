lprn_print_image(gx_device_printer * pdev, gp_file * fp)
{
    gx_device_lprn *const lprn = (gx_device_lprn *) pdev;
    int y;
    int bpl = gdev_mem_bytes_per_scan_line(pdev);
    Bubble *bbtbl;
    Bubble *bbl;
    int i;
    int ri, rmin, read_y;
    int code = 0;
    Bubble *bubbleBuffer;
    int maxBx, maxBy, maxY;
    int start_y_block = 0;	/* start of data in buffer */
    int num_y_blocks = 0;	/* number of data line [r:r+h-1] */

    maxBx = (bpl + lprn->nBw - 1) / lprn->nBw;
    maxBy = (pdev->height + lprn->nBh - 1) / lprn->nBh;
    maxY = lprn->BlockLine / lprn->nBh * lprn->nBh;

    if (!(lprn->ImageBuf = gs_malloc(pdev->memory->non_gc_memory, bpl, maxY, "lprn_print_image(ImageBuf)")))
        return_error(gs_error_VMerror);
    if (!(lprn->TmpBuf = gs_malloc(pdev->memory->non_gc_memory, bpl, maxY, "lprn_print_iamge(TmpBuf)")))
        return_error(gs_error_VMerror);
    if (!(lprn->bubbleTbl = gs_malloc(pdev->memory->non_gc_memory, sizeof(Bubble *), maxBx, "lprn_print_image(bubbleTbl)")))
        return_error(gs_error_VMerror);
    if (!(bubbleBuffer = gs_malloc(pdev->memory->non_gc_memory, sizeof(Bubble), maxBx, "lprn_print_image(bubbleBuffer)")))
        return_error(gs_error_VMerror);

    for (i = 0; i < maxBx; i++)
        lprn->bubbleTbl[i] = NULL;
    bbtbl = bubbleBuffer;

    for (i = 0; i < maxBx - 1; i++)
        bbtbl[i].next = &bbtbl[i + 1];
    bbtbl[i].next = NULL;
    lprn->freeBubbleList = &bbtbl[0];

    for (y = 0; y < maxBy; y++) {
        if (num_y_blocks + lprn->nBh > maxY) {	/* bubble flush for next data */
            rmin = start_y_block + lprn->nBh;	/* process the data under rmin */
            for (i = 0; i < maxBx; i++) {
                bbl = lprn->bubbleTbl[i];
                if (bbl != NULL && bbl->brect.p.y < rmin)
                    lprn_bubble_flush(pdev, fp, bbl);
            }
            num_y_blocks -= lprn->nBh;	/* data if keeps in [r:r+h] */
            start_y_block += lprn->nBh;

        }
        ri = start_y_block + num_y_blocks;	/* read position */
        read_y = ri % maxY;	/* end of read position */
        code = gdev_prn_copy_scan_lines(pdev, ri, lprn->ImageBuf + bpl * read_y, bpl * lprn->nBh);
        if (code < 0)
            return code;

        num_y_blocks += lprn->nBh;

        lprn_process_line(pdev, fp, start_y_block, num_y_blocks);
    }
    lprn_bubble_flush_all(pdev, fp);	/* flush the rest of bubble */

    gs_free(pdev->memory->non_gc_memory, lprn->ImageBuf, bpl, maxY, "lprn_print_image(ImageBuf)");
    gs_free(pdev->memory->non_gc_memory, lprn->TmpBuf, bpl, maxY, "lprn_print_iamge(TmpBuf)");
    gs_free(pdev->memory->non_gc_memory, lprn->bubbleTbl, sizeof(Bubble *), maxBx, "lprn_print_image(bubbleTbl)");
    gs_free(pdev->memory->non_gc_memory, bubbleBuffer, sizeof(Bubble), maxBx, "lprn_print_image(bubbleBuffer)");

    return code;
}