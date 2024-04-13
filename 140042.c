pixMinMaxTiles(PIX     *pixs,
               l_int32  sx,
               l_int32  sy,
               l_int32  mindiff,
               l_int32  smoothx,
               l_int32  smoothy,
               PIX    **ppixmin,
               PIX    **ppixmax)
{
l_int32  w, h;
PIX     *pixmin1, *pixmax1, *pixmin2, *pixmax2;

    PROCNAME("pixMinMaxTiles");

    if (ppixmin) *ppixmin = NULL;
    if (ppixmax) *ppixmax = NULL;
    if (!ppixmin || !ppixmax)
        return ERROR_INT("&pixmin or &pixmax undefined", procName, 1);
    if (!pixs || pixGetDepth(pixs) != 8)
        return ERROR_INT("pixs undefined or not 8 bpp", procName, 1);
    if (pixGetColormap(pixs))
        return ERROR_INT("pixs is colormapped", procName, 1);
    if (sx < 5 || sy < 5)
        return ERROR_INT("sx and/or sy less than 3", procName, 1);
    if (smoothx < 0 || smoothy < 0)
        return ERROR_INT("smooth params less than 0", procName, 1);
    if (smoothx > 5 || smoothy > 5)
        return ERROR_INT("smooth params exceed 5", procName, 1);

        /* Get the min and max values in each tile */
    pixmin1 = pixScaleGrayMinMax(pixs, sx, sy, L_CHOOSE_MIN);
    pixmax1 = pixScaleGrayMinMax(pixs, sx, sy, L_CHOOSE_MAX);

    pixmin2 = pixExtendByReplication(pixmin1, 1, 1);
    pixmax2 = pixExtendByReplication(pixmax1, 1, 1);
    pixDestroy(&pixmin1);
    pixDestroy(&pixmax1);

        /* Make sure no value is 0 */
    pixAddConstantGray(pixmin2, 1);
    pixAddConstantGray(pixmax2, 1);

        /* Generate holes where the contrast is too small */
    pixSetLowContrast(pixmin2, pixmax2, mindiff);

        /* Fill the holes (0 values) */
    pixGetDimensions(pixmin2, &w, &h, NULL);
    pixFillMapHoles(pixmin2, w, h, L_FILL_BLACK);
    pixFillMapHoles(pixmax2, w, h, L_FILL_BLACK);

        /* Smooth if requested */
    if (smoothx > 0 || smoothy > 0) {
        smoothx = L_MIN(smoothx, (w - 1) / 2);
        smoothy = L_MIN(smoothy, (h - 1) / 2);
        *ppixmin = pixBlockconv(pixmin2, smoothx, smoothy);
        *ppixmax = pixBlockconv(pixmax2, smoothx, smoothy);
    }
    else {
        *ppixmin = pixClone(pixmin2);
        *ppixmax = pixClone(pixmax2);
    }
    pixCopyResolution(*ppixmin, pixs);
    pixCopyResolution(*ppixmax, pixs);
    pixDestroy(&pixmin2);
    pixDestroy(&pixmax2);

    return 0;
}