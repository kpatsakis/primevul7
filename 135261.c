GfxTilingPattern *GfxTilingPattern::parse(Object *patObj, int patternRefNum)
{
    Dict *dict;
    int paintTypeA, tilingTypeA;
    double bboxA[4], matrixA[6];
    double xStepA, yStepA;
    Object resDictA;
    Object obj1;
    int i;

    if (!patObj->isStream()) {
        return nullptr;
    }
    dict = patObj->streamGetDict();

    obj1 = dict->lookup("PaintType");
    if (obj1.isInt()) {
        paintTypeA = obj1.getInt();
    } else {
        paintTypeA = 1;
        error(errSyntaxWarning, -1, "Invalid or missing PaintType in pattern");
    }
    obj1 = dict->lookup("TilingType");
    if (obj1.isInt()) {
        tilingTypeA = obj1.getInt();
    } else {
        tilingTypeA = 1;
        error(errSyntaxWarning, -1, "Invalid or missing TilingType in pattern");
    }
    bboxA[0] = bboxA[1] = 0;
    bboxA[2] = bboxA[3] = 1;
    obj1 = dict->lookup("BBox");
    if (obj1.isArray() && obj1.arrayGetLength() == 4) {
        for (i = 0; i < 4; ++i) {
            Object obj2 = obj1.arrayGet(i);
            if (obj2.isNum()) {
                bboxA[i] = obj2.getNum();
            }
        }
    } else {
        error(errSyntaxWarning, -1, "Invalid or missing BBox in pattern");
    }
    obj1 = dict->lookup("XStep");
    if (obj1.isNum()) {
        xStepA = obj1.getNum();
    } else {
        xStepA = 1;
        error(errSyntaxWarning, -1, "Invalid or missing XStep in pattern");
    }
    obj1 = dict->lookup("YStep");
    if (obj1.isNum()) {
        yStepA = obj1.getNum();
    } else {
        yStepA = 1;
        error(errSyntaxWarning, -1, "Invalid or missing YStep in pattern");
    }
    resDictA = dict->lookup("Resources");
    if (!resDictA.isDict()) {
        error(errSyntaxWarning, -1, "Invalid or missing Resources in pattern");
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

    return new GfxTilingPattern(paintTypeA, tilingTypeA, bboxA, xStepA, yStepA, &resDictA, matrixA, patObj, patternRefNum);
}