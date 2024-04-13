pixBackgroundNormGrayArrayMorph(PIX     *pixs,
                                PIX     *pixim,
                                l_int32  reduction,
                                l_int32  size,
                                l_int32  bgval,
                                PIX    **ppixd)
{
l_int32  allfg;
PIX     *pixm;

    PROCNAME("pixBackgroundNormGrayArrayMorph");

    if (!ppixd)
        return ERROR_INT("&pixd not defined", procName, 1);
    *ppixd = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (pixGetDepth(pixs) != 8)
        return ERROR_INT("pixs not 8 bpp", procName, 1);
    if (pixim && pixGetDepth(pixim) != 1)
        return ERROR_INT("pixim not 1 bpp", procName, 1);
    if (reduction < 2 || reduction > 16)
        return ERROR_INT("reduction must be between 2 and 16", procName, 1);

        /* If pixim exists, verify that it is not all foreground. */
    if (pixim) {
        pixInvert(pixim, pixim);
        pixZero(pixim, &allfg);
        pixInvert(pixim, pixim);
        if (allfg)
            return ERROR_INT("pixim all foreground", procName, 1);
    }

    pixGetBackgroundGrayMapMorph(pixs, pixim, reduction, size, &pixm);
    if (!pixm)
        return ERROR_INT("pixm not made", procName, 1);
    *ppixd = pixGetInvBackgroundMap(pixm, bgval, 0, 0);
    pixCopyResolution(*ppixd, pixs);
    pixDestroy(&pixm);
    return 0;
}