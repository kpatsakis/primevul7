pixBackgroundNormRGBArraysMorph(PIX     *pixs,
                                PIX     *pixim,
                                l_int32  reduction,
                                l_int32  size,
                                l_int32  bgval,
                                PIX    **ppixr,
                                PIX    **ppixg,
                                PIX    **ppixb)
{
l_int32  allfg;
PIX     *pixmr, *pixmg, *pixmb;

    PROCNAME("pixBackgroundNormRGBArraysMorph");

    if (!ppixr || !ppixg || !ppixb)
        return ERROR_INT("&pixr, &pixg, &pixb not all defined", procName, 1);
    *ppixr = *ppixg = *ppixb = NULL;
    if (!pixs)
        return ERROR_INT("pixs not defined", procName, 1);
    if (pixGetDepth(pixs) != 32)
        return ERROR_INT("pixs not 32 bpp", procName, 1);
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

    pixGetBackgroundRGBMapMorph(pixs, pixim, reduction, size,
                                &pixmr, &pixmg, &pixmb);
    if (!pixmr || !pixmg || !pixmb) {
        pixDestroy(&pixmr);
        pixDestroy(&pixmg);
        pixDestroy(&pixmb);
        return ERROR_INT("not all pixm* made", procName, 1);
    }

    *ppixr = pixGetInvBackgroundMap(pixmr, bgval, 0, 0);
    *ppixg = pixGetInvBackgroundMap(pixmg, bgval, 0, 0);
    *ppixb = pixGetInvBackgroundMap(pixmb, bgval, 0, 0);
    pixDestroy(&pixmr);
    pixDestroy(&pixmg);
    pixDestroy(&pixmb);
    return 0;
}