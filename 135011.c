GfxColorSpace *GfxLabColorSpace::parse(Array *arr, GfxState *state)
{
    GfxLabColorSpace *cs;
    Object obj1, obj2;

    obj1 = arr->get(1);
    if (!obj1.isDict()) {
        error(errSyntaxWarning, -1, "Bad Lab color space");
        return nullptr;
    }
    cs = new GfxLabColorSpace();
    bool ok = true;
    obj2 = obj1.dictLookup("WhitePoint");
    if (obj2.isArray() && obj2.arrayGetLength() == 3) {
        cs->whiteX = obj2.arrayGet(0).getNum(&ok);
        cs->whiteY = obj2.arrayGet(1).getNum(&ok);
        cs->whiteZ = obj2.arrayGet(2).getNum(&ok);
    }
    obj2 = obj1.dictLookup("BlackPoint");
    if (obj2.isArray() && obj2.arrayGetLength() == 3) {
        cs->blackX = obj2.arrayGet(0).getNum(&ok);
        cs->blackY = obj2.arrayGet(1).getNum(&ok);
        cs->blackZ = obj2.arrayGet(2).getNum(&ok);
    }
    obj2 = obj1.dictLookup("Range");
    if (obj2.isArray() && obj2.arrayGetLength() == 4) {
        cs->aMin = obj2.arrayGet(0).getNum(&ok);
        cs->aMax = obj2.arrayGet(1).getNum(&ok);
        cs->bMin = obj2.arrayGet(2).getNum(&ok);
        cs->bMax = obj2.arrayGet(3).getNum(&ok);
    }

    if (!ok) {
        error(errSyntaxWarning, -1, "Bad Lab color space");
#ifdef USE_CMS
        cs->transform = nullptr;
#endif
        delete cs;
        return nullptr;
    }

    const auto krDenominator = (xyzrgb[0][0] * cs->whiteX + xyzrgb[0][1] * cs->whiteY + xyzrgb[0][2] * cs->whiteZ);
    const auto kgDenominator = (xyzrgb[1][0] * cs->whiteX + xyzrgb[1][1] * cs->whiteY + xyzrgb[1][2] * cs->whiteZ);
    const auto kbDenominator = (xyzrgb[2][0] * cs->whiteX + xyzrgb[2][1] * cs->whiteY + xyzrgb[2][2] * cs->whiteZ);
    if (unlikely(krDenominator == 0 || kgDenominator == 0 || kbDenominator == 0)) {
        delete cs;
        return nullptr;
    }
    cs->kr = 1 / krDenominator;
    cs->kg = 1 / kgDenominator;
    cs->kb = 1 / kbDenominator;

#ifdef USE_CMS
    cs->transform = (state != nullptr) ? state->getXYZ2DisplayTransform() : nullptr;
#endif
    return cs;
}