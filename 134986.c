GfxColor *GfxIndexedColorSpace::mapColorToBase(const GfxColor *color, GfxColor *baseColor) const
{
    unsigned char *p;
    double low[gfxColorMaxComps], range[gfxColorMaxComps];
    int n, i;

    n = base->getNComps();
    base->getDefaultRanges(low, range, indexHigh);
    const int idx = (int)(colToDbl(color->c[0]) + 0.5) * n;
    if (likely((idx + n - 1 < (indexHigh + 1) * base->getNComps()) && idx >= 0)) {
        p = &lookup[idx];
        for (i = 0; i < n; ++i) {
            baseColor->c[i] = dblToCol(low[i] + (p[i] / 255.0) * range[i]);
        }
    } else {
        for (i = 0; i < n; ++i) {
            baseColor->c[i] = 0;
        }
    }
    return baseColor;
}