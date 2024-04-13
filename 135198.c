void GfxState::setFillPattern(GfxPattern *pattern)
{
    if (fillPattern) {
        delete fillPattern;
    }
    fillPattern = pattern;
}