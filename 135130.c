void GfxSeparationColorSpace::getCMYK(const GfxColor *color, GfxCMYK *cmyk) const
{
    double x;
    double c[gfxColorMaxComps];
    GfxColor color2;
    int i;

    if (name->cmp("Black") == 0) {
        cmyk->c = 0;
        cmyk->m = 0;
        cmyk->y = 0;
        cmyk->k = color->c[0];
    } else if (name->cmp("Cyan") == 0) {
        cmyk->c = color->c[0];
        cmyk->m = 0;
        cmyk->y = 0;
        cmyk->k = 0;
    } else if (name->cmp("Magenta") == 0) {
        cmyk->c = 0;
        cmyk->m = color->c[0];
        cmyk->y = 0;
        cmyk->k = 0;
    } else if (name->cmp("Yellow") == 0) {
        cmyk->c = 0;
        cmyk->m = 0;
        cmyk->y = color->c[0];
        cmyk->k = 0;
    } else {
        x = colToDbl(color->c[0]);
        func->transform(&x, c);
        for (i = 0; i < alt->getNComps(); ++i) {
            color2.c[i] = dblToCol(c[i]);
        }
        alt->getCMYK(&color2, cmyk);
    }
}