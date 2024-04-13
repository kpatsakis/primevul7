void GfxDeviceCMYKColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    double c, m, y, k, c1, m1, y1, k1, r, g, b;

    c = colToDbl(color->c[0]);
    m = colToDbl(color->c[1]);
    y = colToDbl(color->c[2]);
    k = colToDbl(color->c[3]);
    c1 = 1 - c;
    m1 = 1 - m;
    y1 = 1 - y;
    k1 = 1 - k;
    cmykToRGBMatrixMultiplication(c, m, y, k, c1, m1, y1, k1, r, g, b);
    rgb->r = clip01(dblToCol(r));
    rgb->g = clip01(dblToCol(g));
    rgb->b = clip01(dblToCol(b));
}