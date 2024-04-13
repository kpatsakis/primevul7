void GfxCalGrayColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    GfxCMYK cmyk;
    clearGfxColor(deviceN);
    getCMYK(color, &cmyk);
    deviceN->c[0] = cmyk.c;
    deviceN->c[1] = cmyk.m;
    deviceN->c[2] = cmyk.y;
    deviceN->c[3] = cmyk.k;
}