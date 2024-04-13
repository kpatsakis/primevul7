void GfxIndexedColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    GfxColor color2;

    base->getRGB(mapColorToBase(color, &color2), rgb);
}