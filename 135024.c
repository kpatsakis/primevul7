GfxPattern *GfxPattern::parse(GfxResources *res, Object *obj, OutputDev *out, GfxState *state, int patternRefNum)
{
    GfxPattern *pattern;
    Object obj1;

    if (obj->isDict()) {
        obj1 = obj->dictLookup("PatternType");
    } else if (obj->isStream()) {
        obj1 = obj->streamGetDict()->lookup("PatternType");
    } else {
        return nullptr;
    }
    pattern = nullptr;
    if (obj1.isInt() && obj1.getInt() == 1) {
        pattern = GfxTilingPattern::parse(obj, patternRefNum);
    } else if (obj1.isInt() && obj1.getInt() == 2) {
        pattern = GfxShadingPattern::parse(res, obj, out, state, patternRefNum);
    }
    return pattern;
}