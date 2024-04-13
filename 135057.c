GfxImageColorMap::GfxImageColorMap(const GfxImageColorMap *colorMap)
{
    int n, i, k;

    colorSpace = colorMap->colorSpace->copy();
    bits = colorMap->bits;
    nComps = colorMap->nComps;
    nComps2 = colorMap->nComps2;
    useMatte = colorMap->useMatte;
    matteColor = colorMap->matteColor;
    colorSpace2 = nullptr;
    for (k = 0; k < gfxColorMaxComps; ++k) {
        lookup[k] = nullptr;
        lookup2[k] = nullptr;
    }
    byte_lookup = nullptr;
    n = 1 << bits;
    for (k = 0; k < nComps; ++k) {
        lookup[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
        memcpy(lookup[k], colorMap->lookup[k], n * sizeof(GfxColorComp));
    }
    if (colorSpace->getMode() == csIndexed) {
        colorSpace2 = ((GfxIndexedColorSpace *)colorSpace)->getBase();
        for (k = 0; k < nComps2; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
            memcpy(lookup2[k], colorMap->lookup2[k], n * sizeof(GfxColorComp));
        }
    } else if (colorSpace->getMode() == csSeparation) {
        colorSpace2 = ((GfxSeparationColorSpace *)colorSpace)->getAlt();
        for (k = 0; k < nComps2; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
            memcpy(lookup2[k], colorMap->lookup2[k], n * sizeof(GfxColorComp));
        }
    } else {
        for (k = 0; k < nComps; ++k) {
            lookup2[k] = (GfxColorComp *)gmallocn(n, sizeof(GfxColorComp));
            memcpy(lookup2[k], colorMap->lookup2[k], n * sizeof(GfxColorComp));
        }
    }
    if (colorMap->byte_lookup) {
        int nc = colorSpace2 ? nComps2 : nComps;

        byte_lookup = (unsigned char *)gmallocn(n, nc);
        memcpy(byte_lookup, colorMap->byte_lookup, n * nc);
    }
    for (i = 0; i < nComps; ++i) {
        decodeLow[i] = colorMap->decodeLow[i];
        decodeRange[i] = colorMap->decodeRange[i];
    }
    ok = true;
}