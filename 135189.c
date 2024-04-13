void GfxDeviceCMYKColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    clearGfxColor(deviceN);
    deviceN->c[0] = clip01(color->c[0]);
    deviceN->c[1] = clip01(color->c[1]);
    deviceN->c[2] = clip01(color->c[2]);
    deviceN->c[3] = clip01(color->c[3]);
}