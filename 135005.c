void GfxLabColorSpace::getDefaultColor(GfxColor *color) const
{
    color->c[0] = 0;
    if (aMin > 0) {
        color->c[1] = dblToCol(aMin);
    } else if (aMax < 0) {
        color->c[1] = dblToCol(aMax);
    } else {
        color->c[1] = 0;
    }
    if (bMin > 0) {
        color->c[2] = dblToCol(bMin);
    } else if (bMax < 0) {
        color->c[2] = dblToCol(bMax);
    } else {
        color->c[2] = 0;
    }
}