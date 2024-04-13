void GfxDeviceGrayColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
    *gray = clip01(color->c[0]);
}