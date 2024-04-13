void GfxDeviceGrayColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    cmyk->c = cmyk->m = cmyk->y = 0;
    cmyk->k = clip01(gfxColorComp1 - color->c[0]);
}