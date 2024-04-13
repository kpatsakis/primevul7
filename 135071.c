void GfxDeviceRGBColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    GfxColorComp c, m, y, k;

    c = clip01(gfxColorComp1 - color->c[0]);
    m = clip01(gfxColorComp1 - color->c[1]);
    y = clip01(gfxColorComp1 - color->c[2]);
    k = c;
    if (m < k) {
        k = m;
    }
    if (y < k) {
        k = y;
    }
    cmyk->c = c - k;
    cmyk->m = m - k;
    cmyk->y = y - k;
    cmyk->k = k;
}