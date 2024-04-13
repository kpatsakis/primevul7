pixGetInvBackgroundMap(PIX     *pixs,
                       l_int32  bgval,
                       l_int32  smoothx,
                       l_int32  smoothy)
{
l_int32    w, h, wplsm, wpld, i, j;
l_int32    val, val16;
l_uint32  *datasm, *datad, *linesm, *lined;
PIX       *pixsm, *pixd;

    PROCNAME("pixGetInvBackgroundMap");

    if (!pixs || pixGetDepth(pixs) != 8)
        return (PIX *)ERROR_PTR("pixs undefined or not 8 bpp", procName, NULL);
    if (pixGetColormap(pixs))
        return (PIX *)ERROR_PTR("pixs has colormap", procName, NULL);
    pixGetDimensions(pixs, &w, &h, NULL);
    if (w < 5 || h < 5)
        return (PIX *)ERROR_PTR("w and h must be >= 5", procName, NULL);

        /* smooth the map image */
    pixsm = pixBlockconv(pixs, smoothx, smoothy);
    datasm = pixGetData(pixsm);
    wplsm = pixGetWpl(pixsm);

        /* invert the map image, scaling up to preserve dynamic range */
    pixd = pixCreate(w, h, 16);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < h; i++) {
        linesm = datasm + i * wplsm;
        lined = datad + i * wpld;
        for (j = 0; j < w; j++) {
            val = GET_DATA_BYTE(linesm, j);
            if (val > 0)
                val16 = (256 * bgval) / val;
            else {  /* shouldn't happen */
                L_WARNING("smoothed bg has 0 pixel!\n", procName);
                val16 = bgval / 2;
            }
            SET_DATA_TWO_BYTES(lined, j, val16);
        }
    }

    pixDestroy(&pixsm);
    pixCopyResolution(pixd, pixs);
    return pixd;
}