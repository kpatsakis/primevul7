pixApplyInvBackgroundGrayMap(PIX     *pixs,
                             PIX     *pixm,
                             l_int32  sx,
                             l_int32  sy)
{
l_int32    w, h, wm, hm, wpls, wpld, i, j, k, m, xoff, yoff;
l_int32    vals, vald;
l_uint32   val16;
l_uint32  *datas, *datad, *lines, *lined, *flines, *flined;
PIX       *pixd;

    PROCNAME("pixApplyInvBackgroundGrayMap");

    if (!pixs || pixGetDepth(pixs) != 8)
        return (PIX *)ERROR_PTR("pixs undefined or not 8 bpp", procName, NULL);
    if (pixGetColormap(pixs))
        return (PIX *)ERROR_PTR("pixs has colormap", procName, NULL);
    if (!pixm || pixGetDepth(pixm) != 16)
        return (PIX *)ERROR_PTR("pixm undefined or not 16 bpp", procName, NULL);
    if (sx == 0 || sy == 0)
        return (PIX *)ERROR_PTR("invalid sx and/or sy", procName, NULL);

    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    pixGetDimensions(pixs, &w, &h, NULL);
    pixGetDimensions(pixm, &wm, &hm, NULL);
    if ((pixd = pixCreateTemplate(pixs)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < hm; i++) {
        lines = datas + sy * i * wpls;
        lined = datad + sy * i * wpld;
        yoff = sy * i;
        for (j = 0; j < wm; j++) {
            pixGetPixel(pixm, j, i, &val16);
            xoff = sx * j;
            for (k = 0; k < sy && yoff + k < h; k++) {
                flines = lines + k * wpls;
                flined = lined + k * wpld;
                for (m = 0; m < sx && xoff + m < w; m++) {
                    vals = GET_DATA_BYTE(flines, xoff + m);
                    vald = (vals * val16) / 256;
                    vald = L_MIN(vald, 255);
                    SET_DATA_BYTE(flined, xoff + m, vald);
                }
            }
        }
    }

    return pixd;
}