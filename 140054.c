pixGetBackgroundGrayMapMorph(PIX     *pixs,
                             PIX     *pixim,
                             l_int32  reduction,
                             l_int32  size,
                             PIX    **ppixm)
{
l_int32    nx, ny, empty, fgpixels;
l_float32  scale;
PIX       *pixm, *pix1, *pix2, *pix3, *pixims;

    PROCNAME("pixGetBackgroundGrayMapMorph");

    if (!ppixm)
        return ERROR_INT("&pixm not defined", procName, 1);
    *ppixm = NULL;
    if (!pixs || pixGetDepth(pixs) != 8)
        return ERROR_INT("pixs not defined or not 8 bpp", procName, 1);
    if (pixGetColormap(pixs))
        return ERROR_INT("pixs is colormapped", procName, 1);
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

        /* Downscale as requested and do the closing to get the background. */
    scale = 1. / (l_float32)reduction;
    pix1 = pixScaleBySampling(pixs, scale, scale);
    pix2 = pixCloseGray(pix1, size, size);
    pix3 = pixExtendByReplication(pix2, 1, 1);
    pixDestroy(&pix1);
    pixDestroy(&pix2);

        /* Downscale the image mask, if any, and remove it from the
         * background.  These pixels will be filled in (twice). */
    pixims = NULL;
    if (pixim) {
        pixims = pixScale(pixim, scale, scale);
        pixm = pixConvertTo8(pixims, FALSE);
        pixAnd(pixm, pixm, pix3);
    }
    else
        pixm = pixClone(pix3);
    pixDestroy(&pix3);

        /* Fill all the holes in the map. */
    nx = pixGetWidth(pixs) / reduction;
    ny = pixGetHeight(pixs) / reduction;
    if (pixFillMapHoles(pixm, nx, ny, L_FILL_BLACK)) {
        pixDestroy(&pixm);
        pixDestroy(&pixims);
        L_WARNING("can't make the map\n", procName);
        return 1;
    }

        /* Finally, for each connected region corresponding to the
         * fg mask, reset all pixels to their average value. */
    if (pixim && fgpixels)
        pixSmoothConnectedRegions(pixm, pixims, 2);
    pixDestroy(&pixims);

    *ppixm = pixm;
    pixCopyResolution(*ppixm, pixs);
    return 0;
}