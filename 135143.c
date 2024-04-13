void GfxDeviceGrayColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    clearGfxColor(deviceN);
    deviceN->c[3] = clip01(gfxColorComp1 - color->c[0]);
}