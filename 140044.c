pixBackgroundNormFlex(PIX     *pixs,
                      l_int32  sx,
                      l_int32  sy,
                      l_int32  smoothx,
                      l_int32  smoothy,
                      l_int32  delta)
{
l_float32  scalex, scaley;
PIX       *pixt, *pixsd, *pixmin, *pixbg, *pixbgi, *pixd;

    PROCNAME("pixBackgroundNormFlex");

    if (!pixs || pixGetDepth(pixs) != 8)
        return (PIX *)ERROR_PTR("pixs undefined or not 8 bpp", procName, NULL);
    if (pixGetColormap(pixs))
        return (PIX *)ERROR_PTR("pixs is colormapped", procName, NULL);
    if (sx < 3 || sy < 3)
        return (PIX *)ERROR_PTR("sx and/or sy less than 3", procName, NULL);
    if (sx > 10 || sy > 10)
        return (PIX *)ERROR_PTR("sx and/or sy exceed 10", procName, NULL);
    if (smoothx < 1 || smoothy < 1)
        return (PIX *)ERROR_PTR("smooth params less than 1", procName, NULL);
    if (smoothx > 3 || smoothy > 3)
        return (PIX *)ERROR_PTR("smooth params exceed 3", procName, NULL);

        /* Generate the bg estimate using smoothed average with subsampling */
    scalex = 1. / (l_float32)sx;
    scaley = 1. / (l_float32)sy;
    pixt = pixScaleSmooth(pixs, scalex, scaley);

        /* Do basin filling on the bg estimate if requested */
    if (delta <= 0)
        pixsd = pixClone(pixt);
    else {
        pixLocalExtrema(pixt, 0, 0, &pixmin, NULL);
        pixsd = pixSeedfillGrayBasin(pixmin, pixt, delta, 4);
        pixDestroy(&pixmin);
    }
    pixbg = pixExtendByReplication(pixsd, 1, 1);

        /* Map the bg to 200 */
    pixbgi = pixGetInvBackgroundMap(pixbg, 200, smoothx, smoothy);
    pixd = pixApplyInvBackgroundGrayMap(pixs, pixbgi, sx, sy);

    pixDestroy(&pixt);
    pixDestroy(&pixsd);
    pixDestroy(&pixbg);
    pixDestroy(&pixbgi);
    return pixd;
}