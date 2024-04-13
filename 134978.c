GfxShadingPattern *GfxShadingPattern::parse(GfxResources *res, Object *patObj, OutputDev *out, GfxState *state, int patternRefNum)
{
    Dict *dict;
    GfxShading *shadingA;
    double matrixA[6];
    Object obj1;
    int i;

    if (!patObj->isDict()) {
        return nullptr;
    }
    dict = patObj->getDict();

    obj1 = dict->lookup("Shading");
    shadingA = GfxShading::parse(res, &obj1, out, state);
    if (!shadingA) {
        return nullptr;
    }

    matrixA[0] = 1;
    matrixA[1] = 0;
    matrixA[2] = 0;
    matrixA[3] = 1;
    matrixA[4] = 0;
    matrixA[5] = 0;
    obj1 = dict->lookup("Matrix");
    if (obj1.isArray() && obj1.arrayGetLength() == 6) {
        for (i = 0; i < 6; ++i) {
            Object obj2 = obj1.arrayGet(i);
            if (obj2.isNum()) {
                matrixA[i] = obj2.getNum();
            }
        }
    }

    return new GfxShadingPattern(shadingA, matrixA, patternRefNum);
}