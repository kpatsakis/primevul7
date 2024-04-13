void GfxDeviceCMYKColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
    *gray = clip01((GfxColorComp)(gfxColorComp1 - color->c[3] - 0.3 * color->c[0] - 0.59 * color->c[1] - 0.11 * color->c[2] + 0.5));
}