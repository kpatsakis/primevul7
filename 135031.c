void GfxImageColorMap::getDeviceN(const unsigned char *x, GfxColor *deviceN)
{
    GfxColor color;
    int i;

    if (colorSpace2) {
        for (i = 0; i < nComps2; ++i) {
            color.c[i] = lookup2[i][x[0]];
        }
        colorSpace2->getDeviceN(&color, deviceN);
    } else {
        for (i = 0; i < nComps; ++i) {
            color.c[i] = lookup[i][x[i]];
        }
        colorSpace->getDeviceN(&color, deviceN);
    }
}