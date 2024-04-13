GfxColorSpace *GfxCalGrayColorSpace::parse(Array *arr, GfxState *state)
{
    GfxCalGrayColorSpace *cs;
    Object obj1, obj2;

    obj1 = arr->get(1);
    if (!obj1.isDict()) {
        error(errSyntaxWarning, -1, "Bad CalGray color space");
        return nullptr;
    }
    cs = new GfxCalGrayColorSpace();
    obj2 = obj1.dictLookup("WhitePoint");
    if (obj2.isArray() && obj2.arrayGetLength() == 3) {
        cs->whiteX = obj2.arrayGet(0).getNumWithDefaultValue(1);
        cs->whiteY = obj2.arrayGet(1).getNumWithDefaultValue(1);
        cs->whiteZ = obj2.arrayGet(2).getNumWithDefaultValue(1);
    }
    obj2 = obj1.dictLookup("BlackPoint");
    if (obj2.isArray() && obj2.arrayGetLength() == 3) {
        cs->blackX = obj2.arrayGet(0).getNumWithDefaultValue(0);
        cs->blackY = obj2.arrayGet(1).getNumWithDefaultValue(0);
        cs->blackZ = obj2.arrayGet(2).getNumWithDefaultValue(0);
    }

    cs->gamma = obj1.dictLookup("Gamma").getNumWithDefaultValue(1);

    cs->kr = 1 / (xyzrgb[0][0] * cs->whiteX + xyzrgb[0][1] * cs->whiteY + xyzrgb[0][2] * cs->whiteZ);
    cs->kg = 1 / (xyzrgb[1][0] * cs->whiteX + xyzrgb[1][1] * cs->whiteY + xyzrgb[1][2] * cs->whiteZ);
    cs->kb = 1 / (xyzrgb[2][0] * cs->whiteX + xyzrgb[2][1] * cs->whiteY + xyzrgb[2][2] * cs->whiteZ);
#ifdef USE_CMS
    cs->transform = (state != nullptr) ? state->getXYZ2DisplayTransform() : nullptr;
#endif
    return cs;
}