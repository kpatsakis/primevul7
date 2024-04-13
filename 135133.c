void GfxPatternColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    clearGfxColor(deviceN);
    deviceN->c[3] = 1;
}