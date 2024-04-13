pixSmoothConnectedRegions(PIX     *pixs,
                          PIX     *pixm,
                          l_int32  factor)
{
l_int32    empty, i, n, x, y;
l_float32  aveval;
BOXA      *boxa;
PIX       *pixmc;
PIXA      *pixa;

    PROCNAME("pixSmoothConnectedRegions");

    if (!pixs || pixGetDepth(pixs) != 8)
        return ERROR_INT("pixs not defined or not 8 bpp", procName, 1);
    if (pixGetColormap(pixs))
        return ERROR_INT("pixs has colormap", procName, 1);
    if (!pixm) {
        L_INFO("pixm not defined\n", procName);
        return 0;
    }
    if (pixGetDepth(pixm) != 1)
        return ERROR_INT("pixm not 1 bpp", procName, 1);
    pixZero(pixm, &empty);
    if (empty) {
        L_INFO("pixm has no fg pixels; nothing to do\n", procName);
        return 0;
    }

    boxa = pixConnComp(pixm, &pixa, 8);
    n = boxaGetCount(boxa);
    for (i = 0; i < n; i++) {
        if ((pixmc = pixaGetPix(pixa, i, L_CLONE)) == NULL) {
            L_WARNING("missing pixmc!\n", procName);
            continue;
        }
        boxaGetBoxGeometry(boxa, i, &x, &y, NULL, NULL);
        pixGetAverageMasked(pixs, pixmc, x, y, factor, L_MEAN_ABSVAL, &aveval);
        pixPaintThroughMask(pixs, pixmc, x, y, (l_int32)aveval);
        pixDestroy(&pixmc);
    }

    boxaDestroy(&boxa);
    pixaDestroy(&pixa);
    return 0;
}