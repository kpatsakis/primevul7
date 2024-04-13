pixApplyVariableGrayMap(PIX     *pixs,
                        PIX     *pixg,
                        l_int32  target)
{
l_int32    i, j, w, h, d, wpls, wplg, wpld, vals, valg, vald;
l_uint8   *lut;
l_uint32  *datas, *datag, *datad, *lines, *lineg, *lined;
l_float32  fval;
PIX       *pixd;

    PROCNAME("pixApplyVariableGrayMap");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (!pixg)
        return (PIX *)ERROR_PTR("pixg not defined", procName, NULL);
    if (!pixSizesEqual(pixs, pixg))
        return (PIX *)ERROR_PTR("pix sizes not equal", procName, NULL);
    pixGetDimensions(pixs, &w, &h, &d);
    if (d != 8)
        return (PIX *)ERROR_PTR("depth not 8 bpp", procName, NULL);

        /* Generate a LUT for the mapping if the image is large enough
         * to warrant the overhead.  The LUT is of size 2^16.  For the
         * index to the table, get the MSB from pixs and the LSB from pixg.
         * Note: this LUT is bigger than the typical 32K L1 cache, so
         * we expect cache misses.  L2 latencies are about 5ns.  But
         * division is slooooow.  For large images, this function is about
         * 4x faster when using the LUT.  C'est la vie.  */
    lut = NULL;
    if (w * h > 100000) {  /* more pixels than 2^16 */
        if ((lut = (l_uint8 *)LEPT_CALLOC(0x10000, sizeof(l_uint8))) == NULL)
            return (PIX *)ERROR_PTR("lut not made", procName, NULL);
        for (i = 0; i < 256; i++) {
            for (j = 0; j < 256; j++) {
                fval = (l_float32)(i * target) / (j + 0.5);
                lut[(i << 8) + j] = L_MIN(255, (l_int32)(fval + 0.5));
            }
        }
    }

    if ((pixd = pixCreateNoInit(w, h, 8)) == NULL) {
        LEPT_FREE(lut);
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    }
    pixCopyResolution(pixd, pixs);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    datag = pixGetData(pixg);
    wplg = pixGetWpl(pixg);
    for (i = 0; i < h; i++) {
        lines = datas + i * wpls;
        lineg = datag + i * wplg;
        lined = datad + i * wpld;
        if (lut) {
            for (j = 0; j < w; j++) {
                vals = GET_DATA_BYTE(lines, j);
                valg = GET_DATA_BYTE(lineg, j);
                vald = lut[(vals << 8) + valg];
                SET_DATA_BYTE(lined, j, vald);
            }
        }
        else {
            for (j = 0; j < w; j++) {
                vals = GET_DATA_BYTE(lines, j);
                valg = GET_DATA_BYTE(lineg, j);
                fval = (l_float32)(vals * target) / (valg + 0.5);
                vald = L_MIN(255, (l_int32)(fval + 0.5));
                SET_DATA_BYTE(lined, j, vald);
            }
        }
    }

    LEPT_FREE(lut);
    return pixd;
}