GfxColorSpace *GfxCalRGBColorSpace::parse(Array *arr, GfxState *state)
{
    GfxCalRGBColorSpace *cs;
    Object obj1, obj2;
    int i;

    obj1 = arr->get(1);
    if (!obj1.isDict()) {
        error(errSyntaxWarning, -1, "Bad CalRGB color space");
        return nullptr;
    }
    cs = new GfxCalRGBColorSpace();
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
    obj2 = obj1.dictLookup("Gamma");
    if (obj2.isArray() && obj2.arrayGetLength() == 3) {
        cs->gammaR = obj2.arrayGet(0).getNumWithDefaultValue(1);
        cs->gammaG = obj2.arrayGet(1).getNumWithDefaultValue(1);
        cs->gammaB = obj2.arrayGet(2).getNumWithDefaultValue(1);
    }
    obj2 = obj1.dictLookup("Matrix");
    if (obj2.isArray() && obj2.arrayGetLength() == 9) {
        for (i = 0; i < 9; ++i) {
            Object obj3 = obj2.arrayGet(i);
            if (likely(obj3.isNum()))
                cs->mat[i] = obj3.getNum();
        }
    }

    cs->kr = 1 / (xyzrgb[0][0] * cs->whiteX + xyzrgb[0][1] * cs->whiteY + xyzrgb[0][2] * cs->whiteZ);
    cs->kg = 1 / (xyzrgb[1][0] * cs->whiteX + xyzrgb[1][1] * cs->whiteY + xyzrgb[1][2] * cs->whiteZ);
    cs->kb = 1 / (xyzrgb[2][0] * cs->whiteX + xyzrgb[2][1] * cs->whiteY + xyzrgb[2][2] * cs->whiteZ);

#ifdef USE_CMS
    cs->transform = (state != nullptr) ? state->getXYZ2DisplayTransform() : nullptr;
#endif
    return cs;
}