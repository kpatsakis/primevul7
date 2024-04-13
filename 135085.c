void GfxIndexedColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
    GfxColor color2;

    base->getGray(mapColorToBase(color, &color2), gray);
}