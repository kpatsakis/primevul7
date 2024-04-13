void GfxIndexedColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    GfxColor color2;

    base->getCMYK(mapColorToBase(color, &color2), cmyk);
}