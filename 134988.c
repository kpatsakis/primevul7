void GfxIndexedColorSpace::getDefaultRanges(double *decodeLow, double *decodeRange, int maxImgPixel) const
{
    decodeLow[0] = 0;
    decodeRange[0] = maxImgPixel;
}