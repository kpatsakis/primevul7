GfxPattern *GfxShadingPattern::copy() const
{
    return new GfxShadingPattern(shading->copy(), matrix, getPatternRefNum());
}