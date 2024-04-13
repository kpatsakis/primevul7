GfxColorSpace *GfxPatternColorSpace::parse(GfxResources *res, Array *arr, OutputDev *out, GfxState *state, int recursion)
{
    GfxPatternColorSpace *cs;
    GfxColorSpace *underA;
    Object obj1;

    if (arr->getLength() != 1 && arr->getLength() != 2) {
        error(errSyntaxWarning, -1, "Bad Pattern color space");
        return nullptr;
    }
    underA = nullptr;
    if (arr->getLength() == 2) {
        obj1 = arr->get(1);
        if (!(underA = GfxColorSpace::parse(res, &obj1, out, state, recursion + 1))) {
            error(errSyntaxWarning, -1, "Bad Pattern color space (underlying color space)");
            return nullptr;
        }
    }
    cs = new GfxPatternColorSpace(underA);
    return cs;
}