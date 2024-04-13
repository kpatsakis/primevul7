pixBackgroundNormRGBArrays(PIX     *pixs,
                           PIX     *pixim,
                           PIX     *pixg,
                           l_int32  sx,
                           l_int32  sy,
                           l_int32  thresh,
                           l_int32  mincount,
                           l_int32  bgval,
                           l_int32  smoothx,
                           l_int32  smoothy,
                           PIX    **ppixr,
                           PIX    **ppixg,
                           PIX    **ppixb)
{
l_int32  allfg;
PIX     *pixmr, *pixmg, *pixmb;

    PROCNAME("pixBackgroundNormRGBArrays");

    if (!ppixr || !ppixg || !ppixb)
        return ERROR_INT("&pixr, &pixg, &pixb not all defined", procName, 1);
    *ppixr = *ppixg = *ppixb = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (pixGetDepth(pixs) != 32)
        return ERROR_INT("pixs not 32 bpp", procName, 1);
    if (pixim && pixGetDepth(pixim) != 1)
        return ERROR_INT("pixim not 1 bpp", procName, 1);
    if (sx < 4 || sy < 4)
        return ERROR_INT("sx and sy must be >= 4", procName, 1);
    if (mincount > sx * sy) {
        L_WARNING("mincount too large for tile size\n", procName);
        mincount = (sx * sy) / 3;
    }

        /* If pixim exists, verify that it is not all foreground. */
    if (pixim) {
        pixInvert(pixim, pixim);
        pixZero(pixim, &allfg);
        pixInvert(pixim, pixim);
        if (allfg)
            return ERROR_INT("pixim all foreground", procName, 1);
    }

    pixGetBackgroundRGBMap(pixs, pixim, pixg, sx, sy, thresh, mincount,
                           &pixmr, &pixmg, &pixmb);
    if (!pixmr || !pixmg || !pixmb) {
        pixDestroy(&pixmr);
        pixDestroy(&pixmg);
        pixDestroy(&pixmb);
        return ERROR_INT("not all pixm* made", procName, 1);
    }

    *ppixr = pixGetInvBackgroundMap(pixmr, bgval, smoothx, smoothy);
    *ppixg = pixGetInvBackgroundMap(pixmg, bgval, smoothx, smoothy);
    *ppixb = pixGetInvBackgroundMap(pixmb, bgval, smoothx, smoothy);
    pixDestroy(&pixmr);
    pixDestroy(&pixmg);
    pixDestroy(&pixmb);
    return 0;
}