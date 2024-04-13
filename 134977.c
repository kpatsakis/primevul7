void GfxIndexedColorSpace::getDeviceN(const GfxColor *color, GfxColor *deviceN) const
{
    GfxColor color2;

    base->getDeviceN(mapColorToBase(color, &color2), deviceN);
}