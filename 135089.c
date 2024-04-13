void GfxColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel) const
{
    int i;

    for (i = 0; i < getNComps(); ++i) {
        decodeLow[i] = 0;
        decodeRange[i] = 1;
    }
}