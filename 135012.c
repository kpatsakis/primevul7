GfxColorSpace *GfxIndexedColorSpace::parse(GfxResources *res, Array *arr, OutputDev *out, GfxState *state, int recursion)
{
    GfxColorSpace *baseA;
    int indexHighA;
    Object obj1;
    const char *s;
    int i, j;

    if (arr->getLength() != 4) {
        error(errSyntaxWarning, -1, "Bad Indexed color space");
        return nullptr;
    }
    obj1 = arr->get(1);
    if (!(baseA = GfxColorSpace::parse(res, &obj1, out, state, recursion + 1))) {
        error(errSyntaxWarning, -1, "Bad Indexed color space (base color space)");
        return nullptr;
    }
    obj1 = arr->get(2);
    if (!obj1.isInt()) {
        error(errSyntaxWarning, -1, "Bad Indexed color space (hival)");
        delete baseA;
        return nullptr;
    }
    indexHighA = obj1.getInt();
    if (indexHighA < 0 || indexHighA > 255) {
        // the PDF spec requires indexHigh to be in [0,255] -- allowing
        // values larger than 255 creates a security hole: if nComps *
        // indexHigh is greater than 2^31, the loop below may overwrite
        // past the end of the array
        int previousValue = indexHighA;
        if (indexHighA < 0)
            indexHighA = 0;
        else
            indexHighA = 255;
        error(errSyntaxWarning, -1, "Bad Indexed color space (invalid indexHigh value, was {0:d} using {1:d} to try to recover)", previousValue, indexHighA);
    }
    GfxIndexedColorSpace *cs = new GfxIndexedColorSpace(baseA, indexHighA);
    obj1 = arr->get(3);
    const int n = baseA->getNComps();
    if (obj1.isStream()) {
        obj1.streamReset();
        for (i = 0; i <= indexHighA; ++i) {
            const int readChars = obj1.streamGetChars(n, &cs->lookup[i * n]);
            for (j = readChars; j < n; ++j) {
                error(errSyntaxWarning, -1, "Bad Indexed color space (lookup table stream too short) padding with zeroes");
                cs->lookup[i * n + j] = 0;
            }
        }
        obj1.streamClose();
    } else if (obj1.isString()) {
        if (obj1.getString()->getLength() < (indexHighA + 1) * n) {
            error(errSyntaxWarning, -1, "Bad Indexed color space (lookup table string too short)");
            goto err3;
        }
        s = obj1.getString()->c_str();
        for (i = 0; i <= indexHighA; ++i) {
            for (j = 0; j < n; ++j) {
                cs->lookup[i * n + j] = (unsigned char)*s++;
            }
        }
    } else {
        error(errSyntaxWarning, -1, "Bad Indexed color space (lookup table)");
        goto err3;
    }
    return cs;

err3:
    delete cs;
    return nullptr;
}