void GfxPatternColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    cmyk->c = cmyk->m = cmyk->y = 0;
    cmyk->k = 1;
}