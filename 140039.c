pixApplyInvBackgroundRGBMap(PIX     *pixs,
                            PIX     *pixmr,
                            PIX     *pixmg,
                            PIX     *pixmb,
                            l_int32  sx,
                            l_int32  sy)
{
l_int32    w, h, wm, hm, wpls, wpld, i, j, k, m, xoff, yoff;
l_int32    rvald, gvald, bvald;
l_uint32   vals;
l_uint32   rval16, gval16, bval16;
l_uint32  *datas, *datad, *lines, *lined, *flines, *flined;
PIX       *pixd;

    PROCNAME("pixApplyInvBackgroundRGBMap");

    if (!pixs)
        return (PIX *)ERROR_PTR("pixs not defined", procName, NULL);
    if (pixGetDepth(pixs) != 32)
        return (PIX *)ERROR_PTR("pixs not 32 bpp", procName, NULL);
    if (!pixmr || !pixmg || !pixmb)
        return (PIX *)ERROR_PTR("pix maps not all defined", procName, NULL);
    if (pixGetDepth(pixmr) != 16 || pixGetDepth(pixmg) != 16 ||
        pixGetDepth(pixmb) != 16)
        return (PIX *)ERROR_PTR("pix maps not all 16 bpp", procName, NULL);
    if (sx == 0 || sy == 0)
        return (PIX *)ERROR_PTR("invalid sx and/or sy", procName, NULL);

    datas = pixGetData(pixs);
    wpls = pixGetWpl(pixs);
    w = pixGetWidth(pixs);
    h = pixGetHeight(pixs);
    wm = pixGetWidth(pixmr);
    hm = pixGetHeight(pixmr);
    if ((pixd = pixCreateTemplate(pixs)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    datad = pixGetData(pixd);
    wpld = pixGetWpl(pixd);
    for (i = 0; i < hm; i++) {
        lines = datas + sy * i * wpls;
        lined = datad + sy * i * wpld;
        yoff = sy * i;
        for (j = 0; j < wm; j++) {
            pixGetPixel(pixmr, j, i, &rval16);
            pixGetPixel(pixmg, j, i, &gval16);
            pixGetPixel(pixmb, j, i, &bval16);
            xoff = sx * j;
            for (k = 0; k < sy && yoff + k < h; k++) {
                flines = lines + k * wpls;
                flined = lined + k * wpld;
                for (m = 0; m < sx && xoff + m < w; m++) {
                    vals = *(flines + xoff + m);
                    rvald = ((vals >> 24) * rval16) / 256;
                    rvald = L_MIN(rvald, 255);
                    gvald = (((vals >> 16) & 0xff) * gval16) / 256;
                    gvald = L_MIN(gvald, 255);
                    bvald = (((vals >> 8) & 0xff) * bval16) / 256;
                    bvald = L_MIN(bvald, 255);
                    composeRGBPixel(rvald, gvald, bvald, flined + xoff + m);
                }
            }
        }
    }

    return pixd;
}