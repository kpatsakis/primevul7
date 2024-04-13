GfxPattern *GfxTilingPattern::copy() const
{
    return new GfxTilingPattern(paintType, tilingType, bbox, xStep, yStep, &resDict, matrix, &contentStream, getPatternRefNum());
}