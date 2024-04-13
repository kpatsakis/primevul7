pixBackgroundNormGrayArray(PIX     *pixs,
                           PIX     *pixim,
                           l_int32  sx,
                           l_int32  sy,
                           l_int32  thresh,
                           l_int32  mincount,
                           l_int32  bgval,
                           l_int32  smoothx,
                           l_int32  smoothy,
                           PIX    **ppixd)
{
l_int32  allfg;
PIX     *pixm;

    PROCNAME("pixBackgroundNormGrayArray");

    if (!ppixd)
        return ERROR_INT("&pixd not defined", procName, 1);
    *ppixd = NULL;
    if (!pixs || pixGetDepth(pixs) != 8)
        return ERROR_INT("pixs not defined or not 8 bpp", procName, 1);
    if (pixGetColormap(pixs))
        return ERROR_INT("pixs is colormapped", procName, 1);
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

    pixGetBackgroundGrayMap(pixs, pixim, sx, sy, thresh, mincount, &pixm);
    if (!pixm)
        return ERROR_INT("pixm not made", procName, 1);
    *ppixd = pixGetInvBackgroundMap(pixm, bgval, smoothx, smoothy);
    pixCopyResolution(*ppixd, pixs);
    pixDestroy(&pixm);
    return 0;
}