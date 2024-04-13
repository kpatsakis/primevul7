pixExtendByReplication(PIX     *pixs,
                       l_int32  addw,
                       l_int32  addh)
{
l_int32   w, h, i, j;
l_uint32  val;
PIX      *pixd;

    PROCNAME("pixExtendByReplication");

    if (!pixs || pixGetDepth(pixs) != 8)
        return (PIX *)ERROR_PTR("pixs undefined or not 8 bpp", procName, NULL);

    if (addw == 0 && addh == 0)
        return pixCopy(NULL, pixs);

    pixGetDimensions(pixs, &w, &h, NULL);
    if ((pixd = pixCreate(w + addw, h + addh, 8)) == NULL)
        return (PIX *)ERROR_PTR("pixd not made", procName, NULL);
    pixRasterop(pixd, 0, 0, w, h, PIX_SRC, pixs, 0, 0);

    if (addw > 0) {
        for (i = 0; i < h; i++) {
            pixGetPixel(pixd, w - 1, i, &val);
            for (j = 0; j < addw; j++)
                pixSetPixel(pixd, w + j, i, val);
        }
    }

    if (addh > 0) {
        for (j = 0; j < w + addw; j++) {
            pixGetPixel(pixd, j, h - 1, &val);
            for (i = 0; i < addh; i++)
                pixSetPixel(pixd, j, h + i, val);
        }
    }

    pixCopyResolution(pixd, pixs);
    return pixd;
}