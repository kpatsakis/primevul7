void GfxSeparationColorSpace::getRGB(const GfxColor *color, GfxRGB *rgb) const
{
    double x;
    double c[gfxColorMaxComps];
    GfxColor color2;
    int i;

    if (alt->getMode() == csDeviceGray && name->cmp("Black") == 0) {
        rgb->r = clip01(gfxColorComp1 - color->c[0]);
        rgb->g = clip01(gfxColorComp1 - color->c[0]);
        rgb->b = clip01(gfxColorComp1 - color->c[0]);
    } else {
        x = colToDbl(color->c[0]);
        func->transform(&x, c);
        const int altNComps = alt->getNComps();
        for (i = 0; i < altNComps; ++i) {
            color2.c[i] = dblToCol(c[i]);
        }
        if (unlikely(altNComps > func->getOutputSize())) {
            for (i = func->getOutputSize(); i < altNComps; ++i) {
                color2.c[i] = 0;
            }
        }
        alt->getRGB(&color2, rgb);
    }
}