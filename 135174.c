void GfxDeviceNColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    double x[gfxColorMaxComps], c[gfxColorMaxComps];
    GfxColor color2;
    int i;

    for (i = 0; i < nComps; ++i) {
        x[i] = colToDbl(color->c[i]);
    }
    func->transform(x, c);
    for (i = 0; i < alt->getNComps(); ++i) {
        color2.c[i] = dblToCol(c[i]);
    }
    alt->getCMYK(&color2, cmyk);
}