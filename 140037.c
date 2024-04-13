pixGetForegroundGrayMap(PIX     *pixs,
                        PIX     *pixim,
                        l_int32  sx,
                        l_int32  sy,
                        l_int32  thresh,
                        PIX    **ppixd)
{
l_int32  w, h, d, wd, hd;
l_int32  empty, fgpixels;
PIX     *pixd, *piximi, *pixim2, *pixims, *pixs2, *pixb, *pixt1, *pixt2, *pixt3;

    PROCNAME("pixGetForegroundGrayMap");

    if (!ppixd)
        return ERROR_INT("&pixd not defined", procName, 1);
    *ppixd = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    pixGetDimensions(pixs, &w, &h, &d);
    if (d != 8)
        return ERROR_INT("pixs not 8 bpp", procName, 1);
    if (pixim && pixGetDepth(pixim) != 1)
        return ERROR_INT("pixim not 1 bpp", procName, 1);
    if (sx < 2 || sy < 2)
        return ERROR_INT("sx and sy must be >= 2", procName, 1);

        /* Generate pixd, which is reduced by the factors (sx, sy). */
    wd = (w + sx - 1) / sx;
    hd = (h + sy - 1) / sy;
    pixd = pixCreate(wd, hd, 8);
    *ppixd = pixd;

        /* Evaluate the 'image' mask, pixim.  If it is all fg,
         * the output pixd has all pixels with value 0. */
    fgpixels = 0;  /* boolean for existence of fg pixels in the image mask. */
    if (pixim) {
        piximi = pixInvert(NULL, pixim);  /* set non-image pixels to 1 */
        pixZero(piximi, &empty);
        pixDestroy(&piximi);
        if (empty)  /* all 'image'; return with all pixels set to 0 */
            return 0;
        pixZero(pixim, &empty);
        if (!empty)  /* there are fg pixels in pixim */
            fgpixels = 1;
    }

        /* 2x subsampling; paint white through 'image' mask. */
    pixs2 = pixScaleBySampling(pixs, 0.5, 0.5);
    if (pixim && fgpixels) {
        pixim2 = pixReduceBinary2(pixim, NULL);
        pixPaintThroughMask(pixs2, pixim2, 0, 0, 255);
        pixDestroy(&pixim2);
    }

        /* Min (erosion) downscaling; total reduction (4 sx, 4 sy). */
    pixt1 = pixScaleGrayMinMax(pixs2, sx, sy, L_CHOOSE_MIN);

/*    pixDisplay(pixt1, 300, 200); */

        /* Threshold to identify fg; paint bg pixels to white. */
    pixb = pixThresholdToBinary(pixt1, thresh);  /* fg pixels */
    pixInvert(pixb, pixb);
    pixPaintThroughMask(pixt1, pixb, 0, 0, 255);
    pixDestroy(&pixb);

        /* Replicative expansion by 2x to (sx, sy). */
    pixt2 = pixExpandReplicate(pixt1, 2);

/*    pixDisplay(pixt2, 500, 200); */

        /* Fill holes in the fg by propagation */
    pixFillMapHoles(pixt2, w / sx, h / sy, L_FILL_WHITE);

/*    pixDisplay(pixt2, 700, 200); */

        /* Smooth with 17x17 kernel. */
    pixt3 = pixBlockconv(pixt2, 8, 8);
    pixRasterop(pixd, 0, 0, wd, hd, PIX_SRC, pixt3, 0, 0);

        /* Paint the image parts black. */
    pixims = pixScaleBySampling(pixim, 1. / sx, 1. / sy);
    pixPaintThroughMask(pixd, pixims, 0, 0, 0);

    pixDestroy(&pixs2);
    pixDestroy(&pixt1);
    pixDestroy(&pixt2);
    pixDestroy(&pixt3);
    return 0;
}