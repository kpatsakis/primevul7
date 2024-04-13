pixLinearTRCTiled(PIX       *pixd,
                  PIX       *pixs,
                  l_int32    sx,
                  l_int32    sy,
                  PIX       *pixmin,
                  PIX       *pixmax)
{
l_int32    i, j, k, m, w, h, wt, ht, wpl, wplt, xoff, yoff;
l_int32    minval, maxval, val, sval;
l_int32   *ia;
l_int32  **iaa;
l_uint32  *data, *datamin, *datamax, *line, *tline, *linemin, *linemax;

    PROCNAME("pixLinearTRCTiled");

    if (!pixs || pixGetDepth(pixs) != 8)
        return (PIX *)ERROR_PTR("pixs undefined or not 8 bpp", procName, pixd);
    if (pixd && pixd != pixs)
        return (PIX *)ERROR_PTR("pixd not null or == pixs", procName, pixd);
    if (pixGetColormap(pixs))
        return (PIX *)ERROR_PTR("pixs is colormapped", procName, pixd);
    if (!pixmin || !pixmax)
        return (PIX *)ERROR_PTR("pixmin & pixmax not defined", procName, pixd);
    if (sx < 5 || sy < 5)
        return (PIX *)ERROR_PTR("sx and/or sy less than 5", procName, pixd);

    if ((iaa = (l_int32 **)LEPT_CALLOC(256, sizeof(l_int32 *))) == NULL)
        return (PIX *)ERROR_PTR("iaa not made", procName, NULL);
    if ((pixd = pixCopy(pixd, pixs)) == NULL) {
        LEPT_FREE(iaa);
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    }
    pixGetDimensions(pixd, &w, &h, NULL);

    data = pixGetData(pixd);
    wpl = pixGetWpl(pixd);
    datamin = pixGetData(pixmin);
    datamax = pixGetData(pixmax);
    wplt = pixGetWpl(pixmin);
    pixGetDimensions(pixmin, &wt, &ht, NULL);
    for (i = 0; i < ht; i++) {
        line = data + sy * i * wpl;
        linemin = datamin + i * wplt;
        linemax = datamax + i * wplt;
        yoff = sy * i;
        for (j = 0; j < wt; j++) {
            xoff = sx * j;
            minval = GET_DATA_BYTE(linemin, j);
            maxval = GET_DATA_BYTE(linemax, j);
            if (maxval == minval) {
                L_ERROR("shouldn't happen! i,j = %d,%d, minval = %d\n",
                        procName, i, j, minval);
                continue;
            }
            if ((ia = iaaGetLinearTRC(iaa, maxval - minval)) == NULL) {
                L_ERROR("failure to make ia for j = %d!\n", procName, j);
                continue;
            }
            for (k = 0; k < sy && yoff + k < h; k++) {
                tline = line + k * wpl;
                for (m = 0; m < sx && xoff + m < w; m++) {
                    val = GET_DATA_BYTE(tline, xoff + m);
                    sval = val - minval;
                    sval = L_MAX(0, sval);
                    SET_DATA_BYTE(tline, xoff + m, ia[sval]);
                }
            }
        }
    }

    for (i = 0; i < 256; i++)
        LEPT_FREE(iaa[i]);
    LEPT_FREE(iaa);
    return pixd;
}