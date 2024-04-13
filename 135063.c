void GfxImageColorMap::getRGB(const unsigned char *x, GfxRGB *rgb)
{
    GfxColor color;
    int i;

    if (colorSpace2) {
        for (i = 0; i < nComps2; ++i) {
            color.c[i] = lookup2[i][x[0]];
        }
        colorSpace2->getRGB(&color, rgb);
    } else {
        for (i = 0; i < nComps; ++i) {
            color.c[i] = lookup2[i][x[i]];
        }
        colorSpace->getRGB(&color, rgb);
    }
}