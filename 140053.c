pixSetLowContrast(PIX     *pixs1,
                  PIX     *pixs2,
                  l_int32  mindiff)
{
l_int32    i, j, w, h, d, wpl, val1, val2, found;
l_uint32  *data1, *data2, *line1, *line2;

    PROCNAME("pixSetLowContrast");

    if (!pixs1 || !pixs2)
        return ERROR_INT("pixs1 and pixs2 not both defined", procName, 1);
    if (pixSizesEqual(pixs1, pixs2) == 0)
        return ERROR_INT("pixs1 and pixs2 not equal size", procName, 1);
    pixGetDimensions(pixs1, &w, &h, &d);
    if (d != 8)
        return ERROR_INT("depth not 8 bpp", procName, 1);
    if (mindiff > 254) return 0;

    data1 = pixGetData(pixs1);
    data2 = pixGetData(pixs2);
    wpl = pixGetWpl(pixs1);
    found = 0;  /* init to not finding any diffs >= mindiff */
    for (i = 0; i < h; i++) {
        line1 = data1 + i * wpl;
        line2 = data2 + i * wpl;
        for (j = 0; j < w; j++) {
            val1 = GET_DATA_BYTE(line1, j);
            val2 = GET_DATA_BYTE(line2, j);
            if (L_ABS(val1 - val2) >= mindiff) {
                found = 1;
                break;
            }
        }
        if (found) break;
    }
    if (!found) {
        L_WARNING("no pixel pair diffs as large as mindiff\n", procName);
        pixClearAll(pixs1);
        pixClearAll(pixs2);
        return 1;
    }

    for (i = 0; i < h; i++) {
        line1 = data1 + i * wpl;
        line2 = data2 + i * wpl;
        for (j = 0; j < w; j++) {
            val1 = GET_DATA_BYTE(line1, j);
            val2 = GET_DATA_BYTE(line2, j);
            if (L_ABS(val1 - val2) < mindiff) {
                SET_DATA_BYTE(line1, j, 0);
                SET_DATA_BYTE(line2, j, 0);
            }
        }
    }

    return 0;
}