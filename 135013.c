void GfxDeviceGrayColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    rgb->r = rgb->g = rgb->b = clip01(color->c[0]);
}