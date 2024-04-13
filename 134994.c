int GfxUnivariateShading::getColor(double t, GfxColor *color)
{
    double out[gfxColorMaxComps];
    int nComps;

    if (likely(getNFuncs() >= 1)) {
        // NB: there can be one function with n outputs or n functions with
        // one output each (where n = number of color components)
        nComps = getNFuncs() * funcs[0]->getOutputSize();
    }

    if (unlikely(getNFuncs() < 1 || nComps > gfxColorMaxComps)) {
        clearGfxColor(color);
        return gfxColorMaxComps;
    }

    if (cacheSize > 0) {
        double x, ix, *l, *u, *upper;

        if (cacheBounds[lastMatch - 1] >= t) {
            upper = std::lower_bound(cacheBounds, cacheBounds + lastMatch - 1, t);
            lastMatch = upper - cacheBounds;
            lastMatch = std::min<int>(std::max<int>(1, lastMatch), cacheSize - 1);
        } else if (cacheBounds[lastMatch] < t) {
            upper = std::lower_bound(cacheBounds + lastMatch + 1, cacheBounds + cacheSize, t);
            lastMatch = upper - cacheBounds;
            lastMatch = std::min<int>(std::max<int>(1, lastMatch), cacheSize - 1);
        }

        x = (t - cacheBounds[lastMatch - 1]) * cacheCoeff[lastMatch];
        ix = 1.0 - x;
        u = cacheValues + lastMatch * nComps;
        l = u - nComps;

        for (int i = 0; i < nComps; ++i) {
            out[i] = ix * l[i] + x * u[i];
        }
    } else {
        for (int i = 0; i < nComps; ++i) {
            out[i] = 0;
        }
        for (int i = 0; i < getNFuncs(); ++i) {
            if (funcs[i]->getInputSize() != 1) {
                error(errSyntaxWarning, -1, "Invalid shading function (input != 1)");
                break;
            }
            funcs[i]->transform(&t, &out[i]);
        }
    }

    for (int i = 0; i < nComps; ++i) {
        color->c[i] = dblToCol(out[i]);
    }
    return nComps;
}