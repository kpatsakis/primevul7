pixBackgroundNorm(PIX     *pixs,
                  PIX     *pixim,
                  PIX     *pixg,
                  l_int32  sx,
                  l_int32  sy,
                  l_int32  thresh,
                  l_int32  mincount,
                  l_int32  bgval,
                  l_int32  smoothx,
                  l_int32  smoothy)
{
l_int32  d, allfg;
PIX     *pixm, *pixmi, *pixd;
PIX     *pixmr, *pixmg, *pixmb, *pixmri, *pixmgi, *pixmbi;

    PROCNAME("pixBackgroundNorm");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    d = pixGetDepth(pixs);
    if (d != 8 && d != 32)
        return (PIX *)ERROR_PTR("pixs not 8 or 32 bpp", procName, NULL);
    if (sx < 4 || sy < 4)
        return (PIX *)ERROR_PTR("sx and sy must be >= 4", procName, NULL);
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
            return (PIX *)ERROR_PTR("pixim all foreground", procName, NULL);
    }

    pixd = NULL;
    if (d == 8) {
        pixm = NULL;
        pixGetBackgroundGrayMap(pixs, pixim, sx, sy, thresh, mincount, &pixm);
        if (!pixm) {
            L_WARNING("map not made; return a copy of the source\n", procName);
            return pixCopy(NULL, pixs);
        }

        pixmi = pixGetInvBackgroundMap(pixm, bgval, smoothx, smoothy);
        if (!pixmi) {
            L_WARNING("pixmi not made; return a copy of source\n", procName);
            pixDestroy(&pixm);
            return pixCopy(NULL, pixs);
        } else {
            pixd = pixApplyInvBackgroundGrayMap(pixs, pixmi, sx, sy);
        }

        pixDestroy(&pixm);
        pixDestroy(&pixmi);
    }
    else {
        pixmr = pixmg = pixmb = NULL;
        pixGetBackgroundRGBMap(pixs, pixim, pixg, sx, sy, thresh,
                               mincount, &pixmr, &pixmg, &pixmb);
        if (!pixmr || !pixmg || !pixmb) {
            pixDestroy(&pixmr);
            pixDestroy(&pixmg);
            pixDestroy(&pixmb);
            L_WARNING("map not made; return a copy of the source\n", procName);
            return pixCopy(NULL, pixs);
        }

        pixmri = pixGetInvBackgroundMap(pixmr, bgval, smoothx, smoothy);
        pixmgi = pixGetInvBackgroundMap(pixmg, bgval, smoothx, smoothy);
        pixmbi = pixGetInvBackgroundMap(pixmb, bgval, smoothx, smoothy);
        if (!pixmri || !pixmgi || !pixmbi) {
            L_WARNING("not all pixm*i are made; return src copy\n", procName);
            pixd = pixCopy(NULL, pixs);
        } else {
            pixd = pixApplyInvBackgroundRGBMap(pixs, pixmri, pixmgi, pixmbi,
                                               sx, sy);
        }

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