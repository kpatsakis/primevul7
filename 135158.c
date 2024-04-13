bool GfxShading::init(GfxResources *res, Dict *dict, OutputDev *out, GfxState *state)
{
    Object obj1;
    int i;

    obj1 = dict->lookup("ColorSpace");
    if (!(colorSpace = GfxColorSpace::parse(res, &obj1, out, state))) {
        error(errSyntaxWarning, -1, "Bad color space in shading dictionary");
        return false;
    }

    for (i = 0; i < gfxColorMaxComps; ++i) {
        background.c[i] = 0;
    }
    hasBackground = false;
    obj1 = dict->lookup("Background");
    if (obj1.isArray()) {
        if (obj1.arrayGetLength() == colorSpace->getNComps()) {
            hasBackground = true;
            for (i = 0; i < colorSpace->getNComps(); ++i) {
                Object obj2 = obj1.arrayGet(i);
                background.c[i] = dblToCol(obj2.getNum());
            }
        } else {
            error(errSyntaxWarning, -1, "Bad Background in shading dictionary");
        }
    }

    bbox_xMin = bbox_yMin = bbox_xMax = bbox_yMax = 0;
    hasBBox = false;
    obj1 = dict->lookup("BBox");
    if (obj1.isArray()) {
        if (obj1.arrayGetLength() == 4) {
            hasBBox = true;
            bbox_xMin = obj1.arrayGet(0).getNum(&hasBBox);
            bbox_yMin = obj1.arrayGet(1).getNum(&hasBBox);
            bbox_xMax = obj1.arrayGet(2).getNum(&hasBBox);
            bbox_yMax = obj1.arrayGet(3).getNum(&hasBBox);
            if (!hasBBox) {
                error(errSyntaxWarning, -1, "Bad BBox in shading dictionary (Values not numbers)");
            }
        } else {
            error(errSyntaxWarning, -1, "Bad BBox in shading dictionary");
        }
    }

    return true;
}