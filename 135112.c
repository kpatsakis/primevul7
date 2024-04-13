void GfxDeviceNColorSpace::getDefaultColor(GfxColor *color) const
{
    int i;

    for (i = 0; i < nComps; ++i) {
        color->c[i] = gfxColorComp1;
    }
}