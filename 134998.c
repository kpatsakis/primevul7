void GfxSeparationColorSpace::getGray(const GfxColor *color, GfxGray *gray) const
{
    double x;
    double c[gfxColorMaxComps];
    GfxColor color2;
    int i;

    if (alt->getMode() == csDeviceGray && name->cmp("Black") == 0) {
        *gray = clip01(gfxColorComp1 - color->c[0]);
    } else {
        x = colToDbl(color->c[0]);
        func->transform(&x, c);
        for (i = 0; i < alt->getNComps(); ++i) {
            color2.c[i] = dblToCol(c[i]);
        }
        alt->getGray(&color2, gray);
    }
}