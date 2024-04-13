pixGetBackgroundRGBMapMorph(PIX     *pixs,
                            PIX     *pixim,
                            l_int32  reduction,
                            l_int32  size,
                            PIX    **ppixmr,
                            PIX    **ppixmg,
                            PIX    **ppixmb)
{
l_int32    nx, ny, empty, fgpixels;
l_float32  scale;
PIX       *pixm, *pixmr, *pixmg, *pixmb, *pix1, *pix2, *pix3, *pixims;

    PROCNAME("pixGetBackgroundRGBMapMorph");

    if (!ppixmr || !ppixmg || !ppixmb)
        return ERROR_INT("&pixm* not all defined", procName, 1);
    *ppixmr = *ppixmg = *ppixmb = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (pixGetDepth(pixs) != 32)
        return ERROR_INT("pixs not 32 bpp", procName, 1);
    if (pixim && pixGetDepth(pixim) != 1)
        return ERROR_INT("pixim not 1 bpp", procName, 1);

        /* Evaluate the mask pixim and make sure it is not all foreground. */
    fgpixels = 0;  /* boolean for existence of fg mask pixels */
    if (pixim) {
        pixInvert(pixim, pixim);  /* set background pixels to 1 */
        pixZero(pixim, &empty);
        if (empty)
            return ERROR_INT("pixim all fg; no background", procName, 1);
        pixInvert(pixim, pixim);  /* revert to original mask */
        pixZero(pixim, &empty);
        if (!empty)  /* there are fg pixels in pixim */
            fgpixels = 1;
    }

        /* Generate an 8 bpp version of the image mask, if it exists */
    scale = 1. / (l_float32)reduction;
    pixims = NULL;
    pixm = NULL;
    if (pixim) {
        pixims = pixScale(pixim, scale, scale);
        pixm = pixConvertTo8(pixims, FALSE);
    }

        /* Downscale as requested and do the closing to get the background.
         * Then remove the image mask pixels from the background.  They
         * will be filled in (twice) later.  Do this for all 3 components. */
    pix1 = pixScaleRGBToGrayFast(pixs, reduction, COLOR_RED);
    pix2 = pixCloseGray(pix1, size, size);
    pix3 = pixExtendByReplication(pix2, 1, 1);
    if (pixim)
        pixmr = pixAnd(NULL, pixm, pix3);
    else
        pixmr = pixClone(pix3);
    pixDestroy(&pix1);
    pixDestroy(&pix2);
    pixDestroy(&pix3);

    pix1 = pixScaleRGBToGrayFast(pixs, reduction, COLOR_GREEN);
    pix2 = pixCloseGray(pix1, size, size);
    pix3 = pixExtendByReplication(pix2, 1, 1);
    if (pixim)
        pixmg = pixAnd(NULL, pixm, pix3);
    else
        pixmg = pixClone(pix3);
    pixDestroy(&pix1);
    pixDestroy(&pix2);
    pixDestroy(&pix3);

    pix1 = pixScaleRGBToGrayFast(pixs, reduction, COLOR_BLUE);
    pix2 = pixCloseGray(pix1, size, size);
    pix3 = pixExtendByReplication(pix2, 1, 1);
    if (pixim)
        pixmb = pixAnd(NULL, pixm, pix3);
    else
        pixmb = pixClone(pix3);
    pixDestroy(&pixm);
    pixDestroy(&pix1);
    pixDestroy(&pix2);
    pixDestroy(&pix3);

        /* Fill all the holes in the three maps. */
    nx = pixGetWidth(pixs) / reduction;
    ny = pixGetHeight(pixs) / reduction;
    if (pixFillMapHoles(pixmr, nx, ny, L_FILL_BLACK) ||
        pixFillMapHoles(pixmg, nx, ny, L_FILL_BLACK) ||
        pixFillMapHoles(pixmb, nx, ny, L_FILL_BLACK)) {
        pixDestroy(&pixmr);
        pixDestroy(&pixmg);
        pixDestroy(&pixmb);
        pixDestroy(&pixims);
        L_WARNING("can't make the maps\n", procName);
        return 1;
    }

        /* Finally, for each connected region corresponding to the
         * fg mask in each component, reset all pixels to their
         * average value. */
    if (pixim && fgpixels) {
        pixSmoothConnectedRegions(pixmr, pixims, 2);
        pixSmoothConnectedRegions(pixmg, pixims, 2);
        pixSmoothConnectedRegions(pixmb, pixims, 2);
        pixDestroy(&pixims);
    }

    *ppixmr = pixmr;
    *ppixmg = pixmg;
    *ppixmb = pixmb;
    pixCopyResolution(*ppixmr, pixs);
    pixCopyResolution(*ppixmg, pixs);
    pixCopyResolution(*ppixmb, pixs);
    return 0;
}