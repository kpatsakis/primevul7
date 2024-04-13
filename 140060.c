pixGlobalNormRGB(PIX     *pixd,
                 PIX     *pixs,
                 l_int32  rval,
                 l_int32  gval,
                 l_int32  bval,
                 l_int32  mapval)
{
l_int32    w, h, d, i, j, ncolors, rv, gv, bv, wpl;
l_int32   *rarray, *garray, *barray;
l_uint32  *data, *line;
NUMA      *nar, *nag, *nab;
PIXCMAP   *cmap;

    PROCNAME("pixGlobalNormRGB");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    cmap = pixGetColormap(pixs);
    pixGetDimensions(pixs, &w, &h, &d);
    if (!cmap && d != 32)
        return (PIX *)ERROR_PTR("pixs not cmapped or 32 bpp", procName, NULL);
    if (mapval <= 0) {
        L_WARNING("mapval must be > 0; setting to 255\n", procName);
        mapval = 255;
    }

        /* Prepare pixd to be a copy of pixs */
    if ((pixd = pixCopy(pixd, pixs)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);

        /* Generate the TRC maps for each component.  Make sure the
         * upper range for each color is greater than zero. */
    nar = numaGammaTRC(1.0, 0, L_MAX(1, 255 * rval / mapval));
    nag = numaGammaTRC(1.0, 0, L_MAX(1, 255 * gval / mapval));
    nab = numaGammaTRC(1.0, 0, L_MAX(1, 255 * bval / mapval));

        /* Extract copies of the internal arrays */
    rarray = numaGetIArray(nar);
    garray = numaGetIArray(nag);
    barray = numaGetIArray(nab);
    if (!nar || !nag || !nab || !rarray || !garray || !barray) {
        L_ERROR("allocation failure in arrays\n", procName);
        goto cleanup_arrays;
    }

    if (cmap) {
        ncolors = pixcmapGetCount(cmap);
        for (i = 0; i < ncolors; i++) {
            pixcmapGetColor(cmap, i, &rv, &gv, &bv);
            pixcmapResetColor(cmap, i, rarray[rv], garray[gv], barray[bv]);
        }
    }
    else {
        data = pixGetData(pixd);
        wpl = pixGetWpl(pixd);
        for (i = 0; i < h; i++) {
            line = data + i * wpl;
            for (j = 0; j < w; j++) {
                extractRGBValues(line[j], &rv, &gv, &bv);
                composeRGBPixel(rarray[rv], garray[gv], barray[bv], line + j);
            }
        }
    }

cleanup_arrays:
    numaDestroy(&nar);
    numaDestroy(&nag);
    numaDestroy(&nab);
    LEPT_FREE(rarray);
    LEPT_FREE(garray);
    LEPT_FREE(barray);
    return pixd;
}