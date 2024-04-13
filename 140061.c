pixBackgroundNormMorph(PIX     *pixs,
                       PIX     *pixim,
                       l_int32  reduction,
                       l_int32  size,
                       l_int32  bgval)
{
l_int32    d, allfg;
PIX       *pixm, *pixmi, *pixd;
PIX       *pixmr, *pixmg, *pixmb, *pixmri, *pixmgi, *pixmbi;

    PROCNAME("pixBackgroundNormMorph");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    d = pixGetDepth(pixs);
    if (d != 8 && d != 32)
        return (PIX *)ERROR_PTR("pixs not 8 or 32 bpp", procName, NULL);
    if (reduction < 2 || reduction > 16)
        return (PIX *)ERROR_PTR("reduction must be between 2 and 16",
                                procName, NULL);

        /* If pixim exists, verify that it is not all foreground. */
    if (pixim) {
        pixInvert(pixim, pixim);
        pixZero(pixim, &allfg);
        pixInvert(pixim, pixim);
        if (allfg)
            return (PIX *)ERROR_PTR("pixim all foreground", procName, NULL);
    }

    pixd = NULL;
    if (d == 8) {
        pixGetBackgroundGrayMapMorph(pixs, pixim, reduction, size, &pixm);
        if (!pixm)
            return (PIX *)ERROR_PTR("pixm not made", procName, NULL);
        pixmi = pixGetInvBackgroundMap(pixm, bgval, 0, 0);
        if (!pixmi)
            ERROR_PTR("pixmi not made", procName, NULL);
        else
            pixd = pixApplyInvBackgroundGrayMap(pixs, pixmi,
                                                reduction, reduction);
        pixDestroy(&pixm);
        pixDestroy(&pixmi);
    }
    else {  /* d == 32 */
        pixmr = pixmg = pixmb = NULL;
        pixGetBackgroundRGBMapMorph(pixs, pixim, reduction, size,
                                    &pixmr, &pixmg, &pixmb);
        if (!pixmr || !pixmg || !pixmb) {
            pixDestroy(&pixmr);
            pixDestroy(&pixmg);
            pixDestroy(&pixmb);
            return (PIX *)ERROR_PTR("not all pixm*", procName, NULL);
        }

        pixmri = pixGetInvBackgroundMap(pixmr, bgval, 0, 0);
        pixmgi = pixGetInvBackgroundMap(pixmg, bgval, 0, 0);
        pixmbi = pixGetInvBackgroundMap(pixmb, bgval, 0, 0);
        if (!pixmri || !pixmgi || !pixmbi)
            ERROR_PTR("not all pixm*i are made", procName, NULL);
        else
            pixd = pixApplyInvBackgroundRGBMap(pixs, pixmri, pixmgi, pixmbi,
                                               reduction, reduction);

        pixDestroy(&pixmr);
        pixDestroy(&pixmg);
        pixDestroy(&pixmb);
        pixDestroy(&pixmri);
        pixDestroy(&pixmgi);
        pixDestroy(&pixmbi);
    }

    if (!pixd)
        ERROR_PTR("pixd not made", procName, NULL);
    pixCopyResolution(pixd, pixs);
    return pixd;
}